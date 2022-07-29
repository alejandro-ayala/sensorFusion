/*
 * HTTPConnection.h
 *
 *  Created on: 20 jul. 2022
 *      Author: Alejandro
 */

#include "lwip/altcp_tcp.h"
#include "lwip/dns.h"
#include "lwip/debug.h"
#include "lwip/mem.h"
#include "lwip/altcp_tls.h"
#include "lwip/init.h"
#include "xil_printf.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include "HTTPConnection.h"

using namespace std;
typedef const char* cstring;

#if LWIP_TCP && LWIP_CALLBACK_API



static err_t httpc_free_state(httpc_state_t* req)
{
	struct altcp_pcb* tpcb;

	if (req->request != NULL)
	{
		pbuf_free(req->request);
		req->request = NULL;
	}

	if (req->rx_hdrs != NULL)
	{
		pbuf_free(req->rx_hdrs);
		req->rx_hdrs = NULL;
	}

	tpcb = req->pcb;
	mem_free(req);
	req = NULL;

	if (tpcb != NULL)
	{
		err_t r;
		altcp_arg(tpcb, NULL);
		altcp_recv(tpcb, NULL);
		altcp_err(tpcb, NULL);
		altcp_poll(tpcb, NULL, 0);
		altcp_sent(tpcb, NULL);
		r = altcp_close(tpcb);
		if (r != ERR_OK)
		{
			altcp_abort(tpcb);
			return ERR_ABRT;
		}
	}
	return ERR_OK;
}

/** Close the connection: call finished callback and free the state */
static err_t httpc_close(httpc_state_t* req, httpc_result_t result, u32_t server_response, err_t err)
{
	if (req != NULL)
	{
		if (req->conn_settings != NULL)
		{
			if (req->conn_settings->result_fn != NULL)
			{
				req->conn_settings->result_fn(req->callback_arg, result, req->rx_content_len, server_response, err);
			}
		}
		return httpc_free_state(req);
	}
	return ERR_OK;
}

/** Parse http header response line 1 */
static err_t http_parse_response_status(struct pbuf *p, u16_t *http_version, u16_t *http_status, u16_t *http_status_str_offset)
{
	u16_t end1 = pbuf_memfind(p, "\r\n", 2, 0);
	if (end1 != 0xFFFF)
	{
		/* get parts of first line */
		u16_t space1, space2;
		space1 = pbuf_memfind(p, " ", 1, 0);
		if (space1 != 0xFFFF)
		{
			if ((pbuf_memcmp(p, 0, "HTTP/", 5) == 0)  && (pbuf_get_at(p, 6) == '.'))
			{
				char status_num[10];
				size_t status_num_len;
				/* parse http version */
				u16_t version = pbuf_get_at(p, 5) - '0';
				version <<= 8;
				version |= pbuf_get_at(p, 7) - '0';
				*http_version = version;

				/* parse http status number */
				space2 = pbuf_memfind(p, " ", 1, space1 + 1);
				if (space2 != 0xFFFF)
				{
					*http_status_str_offset = space2 + 1;
					status_num_len = space2 - space1 - 1;
				}
				else
				{
					status_num_len = end1 - space1 - 1;
				}

				memset(status_num, 0, sizeof(status_num));
				if (pbuf_copy_partial(p, status_num, (u16_t)status_num_len, space1 + 1) == status_num_len)
				{
					int status = atoi(status_num);
					if ((status > 0) && (status <= 0xFFFF))
					{
						*http_status = (u16_t)status;
						return ERR_OK;
					}
				}
			}
		}
	}
	return ERR_VAL;
}

/** Wait for all headers to be received, return its length and content-length (if available) */
static err_t http_wait_headers(struct pbuf *p, u32_t *content_length, u16_t *total_header_len)
{
	u16_t end1 = pbuf_memfind(p, "\r\n\r\n", 4, 0);
	if (end1 < (0xFFFF - 2))
	{
	/* all headers received */
	/* check if we have a content length (@todo: case insensitive?) */
	u16_t content_len_hdr;
	*content_length = HTTPC_CONTENT_LEN_INVALID;
	*total_header_len = end1 + 4;

	content_len_hdr = pbuf_memfind(p, "Content-Length: ", 16, 0);
	if (content_len_hdr != 0xFFFF)
	{
		u16_t content_len_line_end = pbuf_memfind(p, "\r\n", 2, content_len_hdr);
		if (content_len_line_end != 0xFFFF)
		{
			char content_len_num[16];
			u16_t content_len_num_len = (u16_t)(content_len_line_end - content_len_hdr - 16);
			memset(content_len_num, 0, sizeof(content_len_num));
			if (pbuf_copy_partial(p, content_len_num, content_len_num_len, content_len_hdr + 16) == content_len_num_len)
			{
				int len = atoi(content_len_num);

				if ((len >= 0) && ((u32_t)len < HTTPC_CONTENT_LEN_INVALID))
				{
					*content_length = (u32_t)len;
				}
			}
		}
	}
	return ERR_OK;
	}
	return ERR_VAL;
}

/** http client tcp recv callback */
static err_t httpRxCallback(void *arg, struct altcp_pcb *pcb, struct pbuf *p, err_t r)
{
  httpc_state_t* req = (httpc_state_t*)arg;
  LWIP_UNUSED_ARG(r);

  if (p == NULL) {
    httpc_result_t result;
    if (req->parse_state != HTTPC_PARSE_RX_DATA) {
      /* did not get RX data yet */
      result = HTTPC_RESULT_ERR_CLOSED;
    } else if ((req->hdr_content_len != HTTPC_CONTENT_LEN_INVALID) &&
      (req->hdr_content_len != req->rx_content_len)) {
      /* header has been received with content length but not all data received */
      result = HTTPC_RESULT_ERR_CONTENT_LEN;
    } else {
      /* receiving data and either all data received or no content length header */
      result = HTTPC_RESULT_OK;
    }
    return httpc_close(req, result, req->rx_status, ERR_OK);
  }
  if (req->parse_state != HTTPC_PARSE_RX_DATA) {
    if (req->rx_hdrs == NULL) {
      req->rx_hdrs = p;
    } else {
      pbuf_cat(req->rx_hdrs, p);
    }
    if (req->parse_state == HTTPC_PARSE_WAIT_FIRST_LINE) {
      u16_t status_str_off;
      err_t err = http_parse_response_status(req->rx_hdrs, &req->rx_http_version, &req->rx_status, &status_str_off);
      if (err == ERR_OK) {
        /* don't care status string */
        req->parse_state = HTTPC_PARSE_WAIT_HEADERS;
      }
    }
    if (req->parse_state == HTTPC_PARSE_WAIT_HEADERS) {
      u16_t total_header_len;
      err_t err = http_wait_headers(req->rx_hdrs, &req->hdr_content_len, &total_header_len);
      if (err == ERR_OK) {
        struct pbuf *q;
        /* full header received, send window update for header bytes and call into client callback */
        altcp_recved(pcb, total_header_len);
        if (req->conn_settings) {
          if (req->conn_settings->headers_done_fn) {
            err = req->conn_settings->headers_done_fn(req, req->callback_arg, req->rx_hdrs, total_header_len, req->hdr_content_len);
            if (err != ERR_OK) {
              return httpc_close(req, HTTPC_RESULT_LOCAL_ABORT, req->rx_status, err);
            }
          }
        }
        /* hide header bytes in pbuf */
        q = pbuf_free_header(req->rx_hdrs, total_header_len);
        p = q;
        req->rx_hdrs = NULL;
        /* go on with data */
        req->parse_state = HTTPC_PARSE_RX_DATA;
      }
    }
  }
  if ((p != NULL) && (req->parse_state == HTTPC_PARSE_RX_DATA)) {
    req->rx_content_len += p->tot_len;
    if (req->recv_fn != NULL)
    {
      /* directly return here: the connection might already be aborted from the callback! */
      return req->recv_fn(req->callback_arg, pcb, p, r);
    } else {
      altcp_recved(pcb, p->tot_len);
      pbuf_free(p);
    }
  }
  return ERR_OK;
}

static void httpErrorCallback(void *arg, err_t err)
{
	httpc_state_t* req = (httpc_state_t*)arg;
	if (req != NULL)
	{
		/* pcb has already been deallocated */
		req->pcb = NULL;
		httpc_close(req, HTTPC_RESULT_ERR_CLOSED, 0, err);
	}
}

static err_t httpPollingCallback(void *arg, struct altcp_pcb *pcb)
{
	httpc_state_t* req = (httpc_state_t*)arg;
	LWIP_UNUSED_ARG(pcb);
	if (req != NULL)
	{
		if (req->timeout_ticks)
		{
			req->timeout_ticks--;
		}
		if (!req->timeout_ticks)
		{
			return httpc_close(req, HTTPC_RESULT_ERR_TIMEOUT, 0, ERR_OK);
		}
	}
	return ERR_OK;
}

static err_t httpTxCallback(void *arg, struct altcp_pcb *pcb, u16_t len)
{
	/* nothing to do here for now */
	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(len);
	return ERR_OK;
}

/** http client tcp connected callback */
static err_t httpConnectedCallback(void *arg, struct altcp_pcb *pcb, err_t err)
{
	err_t r;
	httpc_state_t* req = (httpc_state_t*)arg;
	LWIP_UNUSED_ARG(pcb);
	LWIP_UNUSED_ARG(err);

	/* send request; last char is zero termination */
	r = altcp_write(req->pcb, req->request->payload, req->request->len - 1, TCP_WRITE_FLAG_COPY);
	if (r != ERR_OK)
	{
		/* could not write the single small request -> fail, don't retry */
		return httpc_close(req, HTTPC_RESULT_ERR_MEM, 0, r);
	}
	/* everything written, we can free the request */
	pbuf_free(req->request);
	req->request = NULL;

	altcp_output(req->pcb);
	return ERR_OK;
}

void endConnCallback(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
	xil_printf("Received HTTP  RESPONSE\n");
}

#endif /* LWIP_TCP && LWIP_CALLBACK_API */


namespace Conectivity
{
HTTPConnection::HTTPConnection(ip_addr_t* serverAddr, uint16_t serverPort) : serverAddr(serverAddr), serverPort(serverPort)
{
	connection = new httpc_state_t();
	settings   = new httpc_connection_t();
	settings->result_fn = endConnCallback;
}

HTTPConnection::~HTTPConnection()
{
	delete connection;
	delete settings;
	_closeConnection();
}


void HTTPConnection::openConnection(altcp_recv_fn recv_fn, const std::string& getReq)
{

	int reqLength =  getReq.length();
	//ASSERT_TECH(ERROR_CODE::HTTP_WRONG_GET_REQ_SIZE, ((reqLength > 0) || (reqLength < 0xFFFF)), "Wrong get request length");
	//ASSERT_INI_NOT_NULL(ERROR_CODE::HTTP_NULL_URL,getReq.c_str());
	//ASSERT_INI_NOT_NULL(ERROR_CODE::HTTP_NULL_URL,req);


	connection->timeout_ticks = HTTPC_POLL_TIMEOUT;
	connection->request = pbuf_alloc(PBUF_RAW, (u16_t)(reqLength + 1), PBUF_RAM);

	connection->hdr_content_len = HTTPC_CONTENT_LEN_INVALID;
	connection->pcb = altcp_new(settings->altcp_allocator);
	if(connection->pcb == NULL)
	{
		//THROW --> during creating TCP control block
	}

	connection->remote_port = serverPort;
	connection->recv_fn = recv_fn;
	connection->conn_settings = settings;
	connection->callback_arg = NULL;
	connection->request->payload = (char*)getReq.c_str();
	connection->remote_addr = *serverAddr;

	altcp_arg(connection->pcb, connection);
	altcp_recv(connection->pcb, httpRxCallback);
	altcp_err(connection->pcb, httpErrorCallback);
	altcp_poll(connection->pcb, httpPollingCallback, HTTPC_POLL_INTERVAL);
	altcp_sent(connection->pcb, httpTxCallback);

	const err_t err = altcp_connect(connection->pcb, &connection->remote_addr, connection->remote_port, httpConnectedCallback);
	if (err == ERR_OK)
	{
		//THROW --> during creating the connection
	}
}

void HTTPConnection::_closeConnection()
{

}

void HTTPConnection::closeConnection()
{
	_closeConnection();
}


void HTTPConnection::httpRequest(const std::string& getReq)
{

	try
	{
		openConnection(httpRxCallback, getReq);
	}
	catch (...)
	{

	}
}

void HTTPConnection::parseGetRequest(const string& url, std::string& getHttpReq)
{
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_IP_ADDR, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_URL, serverIP);

	getHttpReq = "GET " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + HTTPC_CLIENT_AGENT + "\r\n"
		+ "Accept: */*\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Connection: Close\r\n"
		+ "\r\n";
}

void HTTPConnection::parsePostRequest(const string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent)
{
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_IP_ADDR, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_URL, serverIP);

	string msgBody;
	for(const auto& keyValue : msgContent)
	{
        if(!msgBody.empty())
        {
        	msgBody += "&";
        }
		msgBody += keyValue.first + "=" + keyValue.second;
	}

	postHttpReq = "POST " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + HTTPC_CLIENT_AGENT + "\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Content-Type: application/x-www-form-urlencoded\r\n"
		+ "Content-Length: " + std::to_string(msgBody.length()) + "\r\n"
		+ "\r\n"
		+ msgBody
		+ "\r\n";
}

void HTTPConnection::parseJsonPostRequest(const string& url, std::string& postHttpReq, std::vector<pair<string,string>> msgContent)
{
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_IP_ADDR, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	//ASSERT_TEC_NOT_NULL(ERROR_CODE::HTTP_NULL_URL, serverIP);

	stringstream msgBody;
	msgBody <<
    "{" << endl <<
      "\"header\": {" << endl <<
        "\"code\": 0," << endl <<
        "\"timestamp\": \"2022-07-27T07:14:18.996Z\"," << endl <<
        "\"operation\": {" << endl <<
          "\"device\": \"string\"" << endl <<
        "}," << endl <<
        "\"pd\": {" << endl <<
          "\"type\": 0," << endl <<
          "\"value\": 0," << endl <<
          "\"channel\": 0" << endl <<
        "}," << endl <<
        "\"sensor\": {" << endl <<
          "\"type\": 0," << endl <<
          "\"value\": 0," << endl <<
          "\"sensorId\": 0" << endl <<
        "}" << endl <<
      "}" << endl <<
    "}" << endl;
	/*
	for(const auto& keyValue : msgContent)
	{
        if(!msgBody.empty())
        {
        	msgBody += "\",\r\n";
        }
		msgBody += "    \"" + keyValue.first + "\"" + ":\"" + keyValue.second;
	}
	msgBody.insert(0,"{\r\n");
	msgBody += "\"\r\n}";
*/
	postHttpReq = "POST " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + HTTPC_CLIENT_AGENT + "\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Content-Type: application/json\r\n"
		+ "Content-Length: " + std::to_string(msgBody.str().length()) + "\r\n"
		+ "\r\n"
		+ msgBody.str()
		+ "\r\n";
}


}


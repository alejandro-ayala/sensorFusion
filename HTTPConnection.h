/*
 * HTTPConnection.h
 *
 *  Created on: 20 jul. 2022
 *      Author: Alejandro
 */

#ifndef SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_
#define SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "lwip/ip_addr.h"
#include "lwip/altcp.h"

#include "HTTPConnectionTypes.h"


namespace Conectivity
{
class HTTPConnection
{
protected:
	ip_addr_t* serverAddr;
	uint16_t serverPort;
	httpc_state_t* connection;
	httpc_connection_t* settings;

	void _closeConnection();

public:
	HTTPConnection(ip_addr_t* serverAddr, uint16_t serverPort);
	virtual ~HTTPConnection();
	void openConnection(altcp_recv_fn recv_fn, const std::string& getReq);
	void closeConnection();

	void parseGetRequest(const std::string& url, std::string& getHttpReq);
	void parsePostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	void parseJsonPostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	void httpRequest(const std::string& getReq);
};
}

#endif /* SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_ */


#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <fstream>
using std::ifstream;


#include <string.h>
#include <stdint.h>
#include <string.h>

#include "HTTPClient.h"

HTTPClient::HTTPClient()
{
	ip_addr_t* SERVER_IP;
	uint16_t SERVER_PORT;
	IP4_ADDR(SERVER_IP, 192, 168, 1, 100);
	useSSL = false;
	if(useSSL)
	{
		SERVER_PORT = 7233;
	}
	else
	{
		SERVER_PORT = 5233;
	}
	conn = new Conectivity::HTTPConnection(SERVER_IP,SERVER_PORT, useSSL);

}

HTTPClient::~HTTPClient()
{
	closeConnection();
}


void HTTPClient::openConnection()
{
	if(useSSL)
	{
		conn->openConnection();
	}
}

void HTTPClient::closeConnection()
{
	if(useSSL)
	{
		conn->closeConnection();
	}
}


void HTTPClient::getRequest(const char* url)
{
	std::string getHttpReq;
	conn->parseGetRequest(url, getHttpReq);
	conn->request(getHttpReq);
}

void HTTPClient::postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod)
{
	std::string postHttpReq;
	if(postMethod == HTTP_POST_METHOD::Urlencoded)
	{
		conn->parsePostRequest(url, postHttpReq,msgContent);
	}
	else if(postMethod == HTTP_POST_METHOD::Json)
	{
		conn->parseJsonPostRequest(url, postHttpReq,msgContent);
	}
	else if(postMethod == HTTP_POST_METHOD::Multipart)
	{

	}
	conn->request(postHttpReq);
}



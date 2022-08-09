/*
 * HTTPClient.cpp
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */
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
	conn = createNewConnection(true);
}

HTTPClient::~HTTPClient()
{
	closeConnection();
}


void HTTPClient::openConnection()
{
	conn->openConnection();
}

void HTTPClient::closeConnection()
{
	conn->closeConnection();
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

Conectivity::HTTPConnection* HTTPClient::createNewConnection(bool secureConn)
{
	ip_addr_t* serverIP = new ip_addr_t();
	uint16_t serverPort;

	IP4_ADDR(serverIP, 192, 168, 56, 1);

	if(secureConn)
	{
		serverPort = 7233;
		CryptoMng* cryptoMng = new CryptoMng();
		return new Conectivity::HTTPConnection(serverIP,serverPort, cryptoMng);
	}
	else
	{
		serverPort = 5233;
		return new Conectivity::HTTPConnection(serverIP,serverPort);
	}
}

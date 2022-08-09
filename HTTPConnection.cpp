/*
 * HTTPConnection.cpp
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#ifndef TEST_BUILD
#include "lwip/altcp_tcp.h"
#include "lwip/init.h"
#else
#define ipaddr_ntoa(ip) "dummyIP"
#endif
#include "Tools/SPDSocket.h"

#include "HTTPConnection.h"
#include "CryptoMng.h"
#include "Logger.h"
#include "ProjectExceptions.h"

using namespace std;

#define TCP_SERVER_IP_ADDRESS "192.168.56.1"


namespace Conectivity
{
//TODO refactor to pass as pointer ref cryptoMng from client to connection
HTTPConnection::HTTPConnection(ip_addr_t* serverAddr, uint16_t serverPort, CryptoMng* cryptoMng) : serverAddr(serverAddr), serverPort(serverPort), cryptoMng(cryptoMng)
{
		secured = true;
		handshakeDone = false;
}

HTTPConnection::~HTTPConnection()
{
	if(socket) delete socket;
	socket = nullptr;
	closeConnection();
}

Tools::SPDSocket* HTTPConnection::getSocket()
{
	return new Tools::SPDSocket();
}
void HTTPConnection::serverHandshake()
{
	cryptoMng->tlsHandshake();
}

void HTTPConnection::openConnection()
{
	socket = getSocket();
	socket->connectSocket(TCP_SERVER_IP_ADDRESS,serverPort);

	if(secured)
	{
		openTlsConnection();
	}
}

void HTTPConnection::createTLSConfig()
{
	cryptoMng->configureSSL();
}

void HTTPConnection::closeConnection()
{
	if(secured)
	{
		cryptoMng->closeSSLContext();
	}
	socket->disconnectSocket();
}

void HTTPConnection::request(const std::string& getReq)
{
	try
	{
		httpRequest(getReq);
	}
	catch (const FunctionalError& e)
	{
		PRINT_WARNE("Functional error, retry server request", e);
	}
}
void HTTPConnection::httpRequest(const std::string& getReq)
{
	std::string response;
	openConnection();
	sendData(getReq);
	readResponse(response);
	closeConnection();
	return;
}

void HTTPConnection::sendData(const std::string& getReq)
{
	if(secured)
	{
		cryptoMng->sendData(getReq);
	}
	else
	{
		const size_t sentData = socket->writeSocket(getReq.c_str(), getReq.length());

		if(sentData != getReq.length())
		{
			//THROW error
		}
	}
}

void HTTPConnection::readResponse(std::string& response)
{
	char response2[RESPONSE_MAX_SIZE];
	if(secured)
	{
		cryptoMng->readResponse(response);
	}
	else
	{
		const size_t ret = socket->readSocket(response2, RESPONSE_MAX_SIZE);
		response = response2;
	}
}

void HTTPConnection::openTlsConnection()
{
	createTLSConfig();

	if(!handshakeDone)
	{
		cryptoMng->tlsHandshake();
		handshakeDone = true;
	}
}

void HTTPConnection::parseGetRequest(const std::string& url, std::string& getHttpReq)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, serverIP);

	getHttpReq = "GET " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + USER_AGENT + "\r\n"
		+ "Accept: */*\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Connection: Close\r\n"
		+ "\r\n";
}

void HTTPConnection::parsePostRequest(const string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, serverIP);

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
		+ "User-Agent: " + USER_AGENT + "\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Content-Type: application/x-www-form-urlencoded\r\n"
		+ "Content-Length: " + std::to_string(msgBody.length()) + "\r\n"
		+ "\r\n"
		+ msgBody
		+ "\r\n";
}

void HTTPConnection::parseJsonPostRequest(const string& url, std::string& postHttpReq, std::vector<pair<string,string>> msgContent)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());
	cstring serverIP = ipaddr_ntoa(serverAddr);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, serverIP);

	//TODO: implement the real Json parser
	stringstream jsonMsg;
	jsonMsg <<
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


	postHttpReq = "POST " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + USER_AGENT + "\r\n"
		+ "Host: " + serverIP + "\r\n"
		+ "Content-Type: application/json\r\n"
		+ "Content-Length: " + std::to_string(jsonMsg.str().length()) + "\r\n"
		+ "\r\n"
		+ jsonMsg.str()
		+ "\r\n";
}

}

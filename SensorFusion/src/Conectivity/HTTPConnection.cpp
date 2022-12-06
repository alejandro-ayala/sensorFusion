#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#ifndef TEST_BUILD
#include "lwip/altcp_tcp.h"
#include "lwip/init.h"
#else
#define ipaddr_ntoa(ip) "dummyIP"
#endif
#include <Conectivity/SocketController.h>

#include <Conectivity/HTTPConnection.h>
#include <Conectivity/CryptoMng.h>
#include "Tools/Logger.h"
#include "ProjectExceptions.h"

using namespace std;


namespace Conectivity
{
HTTPConnection::HTTPConnection(cstring serverAddr, uint16_t serverPort, CryptoMng* cryptoMng, bool enableSSL) : serverAddr(serverAddr), serverPort(serverPort), cryptoMng(cryptoMng), enableSSL(enableSSL)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, serverAddr);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, serverPort);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, enableSSL);
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, cryptoMng);

	if(enableSSL)
	{
		handshakeDone = false;
	}
}

HTTPConnection::~HTTPConnection()
{
	if(socket) delete socket;
	socket = nullptr;
	closeConnection();
}

SocketController* HTTPConnection::getSocket()
{
	return new SocketController();
}

void HTTPConnection::openConnection()
{
	socket = getSocket();
	socket->connectSocket(serverAddr,serverPort);

	if(enableSSL)
	{
		openSSLConnection();
	}
}

void HTTPConnection::createSSLConfig()
{
	cryptoMng->configureSSL();
}

void HTTPConnection::closeConnection()
{
	if(enableSSL)
	{
		cryptoMng->closeSSLContext();
	}
	socket->disconnectSocket();
}

std::string HTTPConnection::request(const std::string& getReq)
{
	std::string response;
	try
	{
		response = httpRequest(getReq);
	}
	catch (const FunctionalError& e)
	{
		PRINT_WARNE("Functional error, retry server request", e);
	}
	return response;
}
std::string HTTPConnection::httpRequest(const std::string& getReq)
{
	std::string response;

	openConnection();
	sendData(getReq);
	readResponse(response);

	closeConnection();
	return response;
}

void HTTPConnection::sendData(const std::string& getReq)
{
	if(enableSSL)
	{
		cryptoMng->sendData(getReq);
	}
	else
	{
		const size_t sentData = socket->writeSocket(reinterpret_cast<const unsigned char *>(getReq.c_str()), getReq.length());

		if(sentData != getReq.length())
		{
			THROW_FUNC_EXCEPT(ERROR_CODE::HTTP_INCOMPLETE_SEND, "The full request was not sent");
		}
	}
}

void HTTPConnection::readResponse(std::string& response)
{
	char response2[RESPONSE_MAX_SIZE];
	if(enableSSL)
	{
		cryptoMng->readResponse(response);
	}
	else
	{
		const size_t ret = socket->readSocket(reinterpret_cast<unsigned char *>(response2), RESPONSE_MAX_SIZE);
		response = response2;
	}
}

void HTTPConnection::openSSLConnection()
{
	createSSLConfig();

	if(!handshakeDone)
	{
		cryptoMng->handshakeSSL();
		handshakeDone = true;
	}
}

void HTTPConnection::parseGetRequest(const std::string& url, std::string& getHttpReq)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());

	getHttpReq = "GET " + url + " HTTP/1.1\r\n"
		+ "User-Agent: " + USER_AGENT + "\r\n"
		+ "Accept: */*\r\n"
		+ "Host: " + serverAddr + "\r\n"
		+ "Connection: Close\r\n"
		+ "\r\n";
}

void HTTPConnection::parsePostRequest(const string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());

	if(postMethod == HTTP_POST_METHOD::Urlencoded)
	{
		parseURLPostRequest(url, postHttpReq,msgContent);
	}
	else if(postMethod == HTTP_POST_METHOD::Json)
	{
		parseJsonPostRequest(url, postHttpReq,msgContent);
	}
	else if(postMethod == HTTP_POST_METHOD::Multipart)
	{
		//TODO:
	}
}

void HTTPConnection::parseURLPostRequest(const string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());

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
		+ "Host: " + serverAddr + "\r\n"
		+ "Content-Type: application/x-www-form-urlencoded\r\n"
		+ "Content-Length: " + std::to_string(msgBody.length()) + "\r\n"
		+ "\r\n"
		+ msgBody
		+ "\r\n";
}

void HTTPConnection::parseJsonPostRequest(const string& url, std::string& postHttpReq, std::vector<pair<string,string>> msgContent)
{
	ASSERT_TEC_NOT_NULL(ERROR_CODE::NULL_PARAMETER, url.c_str());

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
		+ "Host: " + serverAddr + "\r\n"
		+ "Content-Type: application/json\r\n"
		+ "Content-Length: " + std::to_string(jsonMsg.str().length()) + "\r\n"
		+ "\r\n"
		+ jsonMsg.str()
		+ "\r\n";
}

}

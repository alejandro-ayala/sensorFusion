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
	conn = createNewConnection(false);
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

std::string HTTPClient::getRequest(const char* url)
{
	std::string getHttpReq;
	conn->parseGetRequest(url, getHttpReq);
	std::string response = conn->request(getHttpReq);
	return response;
}

void HTTPClient::postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod)
{
	std::string postHttpReq;
	conn->parsePostRequest(url, postHttpReq,msgContent, postMethod);
	conn->request(postHttpReq);
}

Conectivity::HTTPConnection* HTTPClient::createNewConnection(bool secureConn)
{
	cstring serverIP = "192.168.0.20";
	uint16_t serverPort;
	CryptoConfig cryptoConfig = {false,false,false,serverIP};
	static CryptoMng* cryptoMng = new CryptoMng(cryptoConfig);

	if(secureConn)
	{
		serverPort = 7233;
	}
	else
	{
		serverPort = 5233;
	}
	return new Conectivity::HTTPConnection(serverIP,serverPort, cryptoMng, secureConn);

}

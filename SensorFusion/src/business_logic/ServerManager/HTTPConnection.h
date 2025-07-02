#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#ifndef TEST_BUILD
#include "lwip/ip_addr.h"
#include "lwip/altcp.h"
#endif
#include "HTTPConnectionTypes.h"
#include "CryptoMng.h"

#include "SocketHandler.h"

class HTTPConnection
{
private:
	std::string serverAddr;
	uint16_t serverPort;
	CryptoMng* cryptoMng;
	bool enableSSL;
	bool handshakeDone;
	SocketHandler* socket;

	void createSSLConfig();
	void openSSLConnection();
	std::string httpRequest(const std::string& getReq);
	void sendData(const std::string& getReq);
	void readResponse(std::string& response);
	void parseURLPostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	void parseJsonPostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	SocketHandler* getSocket();

public:
	HTTPConnection(const std::string& serverAddr, uint16_t serverPort, CryptoMng* cryptoMng, bool enableSSL);
	virtual ~HTTPConnection();
	void openConnection();
	void closeConnection();


	void parsePostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	void parseGetRequest(const std::string& url, std::string& getHttpReq);
	std::string request(const std::string& getReq);

};


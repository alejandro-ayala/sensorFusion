/*
 * HTTPConnection.h
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */

#ifndef SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_
#define SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_
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
#include "TestInclude.h"

namespace Tools
{
	class SPDSocket;
}

namespace Conectivity
{
class HTTPConnection
{
protected:
	ip_addr_t* serverAddr;
	uint16_t serverPort;
	Tools::SPDSocket* socket;
	bool secured;
	bool handshakeDone;
	CryptoMng* cryptoMng;

	void httpRequest(const std::string& getReq);
	void sendData(const std::string& getReq);
	void readResponse(std::string& response);
	Tools::SPDSocket* getSocket();

public:
	HTTPConnection(ip_addr_t* serverAddr, uint16_t serverPort, CryptoMng* cryptoMng);
	HTTPConnection(ip_addr_t* serverAddr, uint16_t serverPort) : serverAddr(serverAddr), serverPort(serverPort), secured(false) {};
	virtual ~HTTPConnection();
	TVIRTUAL void openConnection();
	TVIRTUAL void closeConnection();
	TVIRTUAL void createTLSConfig();
	TVIRTUAL void serverHandshake();
	TVIRTUAL void openTlsConnection();

	TVIRTUAL void parseGetRequest(const std::string& url, std::string& getHttpReq);
	TVIRTUAL void parsePostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	TVIRTUAL void parseJsonPostRequest(const std::string& url, std::string& postHttpReq, std::vector<std::pair<std::string,std::string>> msgContent);
	TVIRTUAL void request(const std::string& getReq);

};
}

#endif /* SRC_CONECTIVITY_HTTP_HTTPCONNECTION_H_ */

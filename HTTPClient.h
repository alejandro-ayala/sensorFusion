

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HTTPConnection.h"
using namespace Conectivity;

class HTTPClient
{
protected:

	bool logged;
	HTTPConnection* conn;
	uint32_t proxy_addr;
	uint16_t proxy_port;
	uint8_t use_proxy;

	void _closeConnection();

public:
	HTTPClient();
	virtual ~HTTPClient();

	void openConnection();
	void closeConnection();

	void getRequest(const char* url);
	void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	void createTLSConfig();
	void readRootCA();
};


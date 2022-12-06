#pragma once

#include <string>
#include "HTTPConnection.h"

class HTTPClient
{
private:
	Conectivity::HTTPConnection* conn;

public:
	HTTPClient();
	virtual ~HTTPClient();

	TVIRTUAL void openConnection();
	TVIRTUAL void closeConnection();

	TVIRTUAL std::string getRequest(const char* url);
	TVIRTUAL void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	TVIRTUAL Conectivity::HTTPConnection* createNewConnection(bool secureConn);
};


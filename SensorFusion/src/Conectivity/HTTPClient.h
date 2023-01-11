#pragma once

#include <string>
#include "HTTPConnection.h"
namespace Conectivity
{
class HTTPClient
{
private:
	Conectivity::HTTPConnection* conn;

public:
	HTTPClient();
	virtual ~HTTPClient();

	void openConnection();
	void closeConnection();

	std::string getRequest(const char* url);
	void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	Conectivity::HTTPConnection* createNewConnection(bool secureConn);
};
}


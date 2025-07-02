#pragma once

#include <business_logic/ServerManager/HTTPConnection.h>
#include <string>

class HTTPClient
{
private:
	HTTPConnection* conn;

public:
	HTTPClient();
	virtual ~HTTPClient();

	void openConnection();
	void closeConnection();

	std::string getRequest(const char* url);
	void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	HTTPConnection* createNewConnection(bool secureConn);
};


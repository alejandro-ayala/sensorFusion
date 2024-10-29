#pragma once

#include <business_logic/Conectivity/HTTPConnection.h>
#include <string>

namespace business_logic
{
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
}

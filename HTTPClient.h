

#include <string>
#include "HTTPConnection.h"

class HTTPClient
{
protected:
	Conectivity::HTTPConnection* conn;
	uint8_t useSSL;

public:
	HTTPClient();
	virtual ~HTTPClient();

	void openConnection();
	void closeConnection();

	void getRequest(const char* url);
	void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
};


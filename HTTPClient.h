/*
 * HTTPClient.h
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */

#include <string>
#include "HTTPConnection.h"

class HTTPClient
{
protected:
	Conectivity::HTTPConnection* conn;

public:
	HTTPClient();
	virtual ~HTTPClient();

	TVIRTUAL void openConnection();
	TVIRTUAL void closeConnection();

	TVIRTUAL void getRequest(const char* url);
	TVIRTUAL void postRequest(const char* url,std::vector<std::pair<std::string,std::string>> msgContent, const HTTP_POST_METHOD& postMethod);
	TVIRTUAL Conectivity::HTTPConnection* createNewConnection(bool secureConn);
};


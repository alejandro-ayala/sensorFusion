#pragma once

#include "HTTPClient.h"

namespace Conectivity
{
class ServerManager
{
private:
	HTTPClient* client;
public:
	ServerManager();
	virtual ~ServerManager();
	std::string getConfiguration();
	void initialization();
	void sendReport();
};
}

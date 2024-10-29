#pragma once

#include <business_logic/Conectivity/HTTPClient.h>

namespace business_logic
{
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
}

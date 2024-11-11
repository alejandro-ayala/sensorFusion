
#include <business_logic/Conectivity/ServerManager.h>
#include "services/Logger/LoggerMacros.h"
namespace business_logic
{
namespace Conectivity
{
ServerManager::ServerManager()
{
	client = new HTTPClient();
}

ServerManager::~ServerManager()
{
	delete client;
}

void ServerManager::initialization()
{

}

std::string ServerManager::getConfiguration()
{
	LOG_TRACE("getConfiguration\r\n");
	std::string response = client->getRequest("/configuration");
	return response;
}
void ServerManager::sendReport()
{
	LOG_TRACE("sendReport\r\n");
	std::vector<std::pair<std::string,std::string>> msgContent;
	auto p1 = std::make_pair("sensor1","value1");
	msgContent.push_back(p1);
	client->postRequest("/report",msgContent,HTTP_POST_METHOD::Json);
}
}
}

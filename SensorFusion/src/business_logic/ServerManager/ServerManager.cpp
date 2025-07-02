
#include "ServerManager.h"
#include "services/Logger/LoggerMacros.h"

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
	LOG_DEBUG("getConfiguration");
	std::string response = client->getRequest("/configuration");
	return response;
}
void ServerManager::sendData()
{
	LOG_DEBUG("sendData\r\n");
	std::vector<std::pair<std::string,std::string>> msgContent;
	auto p1 = std::make_pair("sensor1","value1");
	msgContent.push_back(p1);
	client->postRequest("/report",msgContent,HTTP_POST_METHOD::Json);
}


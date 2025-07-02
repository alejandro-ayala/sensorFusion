#pragma once

#include "HTTPClient.h"


class ServerManager
{
private:
	HTTPClient* client;
public:
	ServerManager();
	virtual ~ServerManager();
	std::string getConfiguration();
	void initialization();
	void sendData();
};


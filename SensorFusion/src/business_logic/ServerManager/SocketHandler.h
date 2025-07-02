#pragma once
#include <string>

class SocketHandler
{
protected:
	int socket;

public:
	SocketHandler();
	virtual ~SocketHandler();

	void connectSocket(unsigned char* ip, uint16_t port);
	void disconnectSocket();
	size_t writeSocket(const unsigned char *buf, size_t len);
	size_t readSocket(unsigned char *buf, size_t len);

};


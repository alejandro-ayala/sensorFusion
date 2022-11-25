
#include "SPDSocket.h"
#include <lwip/sockets.h>
namespace Tools
{

SPDSocket::SPDSocket()
{

}

SPDSocket::~SPDSocket()
{

}

void SPDSocket::connectSocket(unsigned char* ip, uint16_t port)
{
	socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = inet_addr(ip);

	if (connect(socket, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		//xil_printf("TCP Client: Error on tcp_connect\r\n");
		close(socket);
	}
}

void SPDSocket::disconnectSocket()
{
	close(socket);
}

size_t SPDSocket::writeSocket(const unsigned char *buf, size_t len)
{
	socket = 0;
	int ret = send(socket,buf, len,0);

	return ret;
}

size_t SPDSocket::readSocket(unsigned char *buf, size_t len)
{
	socket = 0;
	int ret = recv(socket,buf, len,0);
	return ret;
}

}

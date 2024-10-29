
#include <business_logic/Conectivity/SocketController.h>
#include <lwip/sockets.h>


namespace business_logic
{
namespace Conectivity
{

SocketController::SocketController()
{

}

SocketController::~SocketController()
{

}

void SocketController::connectSocket(cstring ip, uint16_t port)
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

void SocketController::disconnectSocket()
{
	close(socket);
}

size_t SocketController::writeSocket(const unsigned char *buf, size_t len)
{
	socket = 0;
	int ret = send(socket,buf, len,0);

	return ret;
}

size_t SocketController::readSocket(unsigned char *buf, size_t len)
{
	socket = 0;
	int ret = recv(socket,buf, len,0);
	return ret;
}
}
}

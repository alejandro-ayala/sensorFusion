#pragma once
#include <string>
#include "Definitions.h"

namespace business_logic
{
namespace Conectivity
{
class SocketController
{
protected:
	int socket;

public:
	SocketController();
	virtual ~SocketController();

	void connectSocket(cstring ip, uint16_t port);
	void disconnectSocket();
	size_t writeSocket(const unsigned char *buf, size_t len);
	size_t readSocket(unsigned char *buf, size_t len);

};
}
}

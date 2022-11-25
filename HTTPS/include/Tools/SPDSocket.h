/*
 * SPDSocket.h
 *
 *  Created on: 25 jul. 2022
 *      Author: Alejandro
 */

#include <string>
#include "TestInclude.h"

namespace Tools
{
class SPDSocket
{
protected:
	int socket;

public:
	SPDSocket();
	virtual ~SPDSocket();

	TVIRTUAL void connectSocket(unsigned char* ip, uint16_t port);
	TVIRTUAL void disconnectSocket();
	TVIRTUAL size_t writeSocket(const unsigned char *buf, size_t len);
	TVIRTUAL size_t readSocket(unsigned char *buf, size_t len);

};

}

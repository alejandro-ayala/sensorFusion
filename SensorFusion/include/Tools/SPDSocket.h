#include <string>
#include "TestInclude.h"
#include "Definitions.h"

namespace Tools
{
class SPDSocket
{
protected:
	int socket;

public:
	SPDSocket();
	virtual ~SPDSocket();

	TVIRTUAL void connectSocket(cstring ip, uint16_t port);
	TVIRTUAL void disconnectSocket();
	TVIRTUAL size_t writeSocket(const unsigned char *buf, size_t len);
	TVIRTUAL size_t readSocket(unsigned char *buf, size_t len);

};

}

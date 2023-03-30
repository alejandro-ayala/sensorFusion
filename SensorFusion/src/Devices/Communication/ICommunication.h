#pragma once
#include <stdint.h>
#include "Controllers/IController.h"

namespace Components
{
namespace Communication
{
class ICommunication : public Controllers::IController
{
public:
	ICommunication(cstring name) : Controllers::IController(name) {};
	virtual ~ICommunication(){};

	virtual int transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength) = 0;
	virtual int receiveMsg(uint8_t *rxMsg) = 0;
};
}
}

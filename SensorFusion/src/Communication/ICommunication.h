#pragma once
#include <stdint.h>
#include "HW/IController.h"

namespace Communication
{
class ICommunication : public Hardware::IController
{
public:
	ICommunication(cstring name) : Hardware::IController(name) {};
	virtual ~ICommunication(){};

	virtual int transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength) = 0;
	virtual int receiveMsg(uint8_t *rxMsg) = 0;
};
}

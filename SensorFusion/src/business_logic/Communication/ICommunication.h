#pragma once
#include <hardware_abstraction/Controllers/IController.h>
#include <stdint.h>

namespace business_logic
{
namespace Communication
{
class ICommunication : public hardware_abstraction::Controllers::IController
{
public:
	ICommunication(cstring name) : hardware_abstraction::Controllers::IController(name) {};
	virtual ~ICommunication(){};

	virtual int transmitMsg(uint8_t idMsg, uint8_t *txMsg, uint8_t msgLength) = 0;
	virtual int receiveMsg(uint8_t *rxMsg) = 0;
};
}
}

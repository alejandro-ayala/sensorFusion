#pragma once
#include "TestInclude.h"
#include "xadcps.h"
#include "HW/IController.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"

namespace Communication
{
class CanController : public Hardware::IController
{
private:
	int deviceId;
	XCanPs *CanInstPtr;
	XCanPs_Config *ConfigPtr;
public:
	CanController();
	virtual ~CanController();

	virtual void initialize() override;
	int sendFrame();
	int receiveFrame();
	bool selfTest();
};
}

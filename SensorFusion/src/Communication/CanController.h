#pragma once
#include "TestInclude.h"
#include "xadcps.h"
#include "xcanps.h"
#include "xparameters.h"
#include "xil_printf.h"

namespace Communication
{
class CanController
{
private:
	int deviceId;
	XCanPs *CanInstPtr;
	XCanPs_Config *ConfigPtr;
public:
	CanController();
	virtual ~CanController();

	int initialization();
	int sendFrame();
	int receiveFrame();
	bool selfTest();
};
}

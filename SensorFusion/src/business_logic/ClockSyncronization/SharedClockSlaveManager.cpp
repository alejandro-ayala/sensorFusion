#include <business_logic/ClockSyncronization/CanSyncMessages.h>
#include <business_logic/ClockSyncronization/SharedClockSlaveManager.h>
#include <business_logic/Communication/CanIDs.h>
#include "hardware_abstraction/Controllers/CAN/CanController.h"
#include "services/Logger/LoggerMacros.h"
#include <iostream>

namespace business_logic
{
using namespace hardware_abstraction::Controllers;
using namespace Communication;
namespace ClockSyncronization
{
SharedClockSlaveManager::SharedClockSlaveManager(TimeController* timecontroller, CanController* cancontroller) : timeController(timecontroller), canController(cancontroller)
{

}

SharedClockSlaveManager::~SharedClockSlaveManager() {}

void SharedClockSlaveManager::initialization()
{
	timeController->initialize();
	canController->initialize();
	timeController->startTimer();
}

bool SharedClockSlaveManager::getGlobalTime()
{
	uint8_t cnt = 0;
	uint8_t lenght = 800;
	uint8_t data[lenght];
	bool updatedTime;
	CanSyncMessage globalTimeSynMsg;
	CanFUPMessage  globalTimeFupMsg;
	const auto rxMsg = canController->receiveMsg();
	if(rxMsg.dlc != 0 && data[0] == static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC))
	{
		if(data[1] == static_cast<uint8_t>(SYNC_MSG_TYPES::SYNC_MSG))
		{
			globalTimeSynMsg.type        = data[1];
			globalTimeSynMsg.crc         = data[2];
			globalTimeSynMsg.secCounter  = data[3];
			globalTimeSynMsg.userByte    = data[4];
			globalTimeSynMsg.syncTimeSec = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
//			xil_printf(
//					"SYNC -- type: %02x, crc: %02x, cnt: %02x, userByte: %02x, sec: %02x \n",
//					globalTimeSynMsg.type, globalTimeSynMsg.crc,
//					globalTimeSynMsg.secCounter, globalTimeSynMsg.userByte,
//					globalTimeSynMsg.syncTimeSec);
			globalTimeStamp.seconds = globalTimeSynMsg.syncTimeSec;
			updatedTime = false;
		}
		else if(data[1] == static_cast<uint8_t>(SYNC_MSG_TYPES::FUP_MSG))
		{
			globalTimeFupMsg.type        = data[1];
			globalTimeFupMsg.crc         = data[2];
			globalTimeFupMsg.secCounter  = data[3];
			globalTimeFupMsg.overflowSeconds    = data[4];
			globalTimeFupMsg.syncTimeNSec = data[5] << 24 | data[6] << 16 | data[7] << 8 | data[8];
//			xil_printf(
//					"FUP -- type: %02x, crc: %02x, cnt: %02x, ovf: %02x, nsec: %08x \n",
//					globalTimeFupMsg.type, globalTimeFupMsg.crc,
//					globalTimeFupMsg.secCounter,
//					globalTimeFupMsg.overflowSeconds,
//					globalTimeFupMsg.syncTimeNSec);

			globalTimeStamp.nanoseconds=globalTimeFupMsg.syncTimeNSec;

			uint64_t currentGT = timeController->getLocalTime();
			uint64_t newGT = (globalTimeStamp.seconds * 1e9) + globalTimeStamp.nanoseconds;
			//LOG_DEBUG("newGT: ", newGT,  "currentGT: ",  currentGT);
			timeController->setGlobalTimeReference(globalTimeStamp);
			updatedTime = true;
		}
		else
		{
			LOG_WARNING("Wrong MSG: size = " , std::to_string(rxMsg.dlc));
			for(int i=0;i<rxMsg.dlc;i++)
			{
				LOG_WARNING(data[i], ",");
			}
			updatedTime = false;
		}
	}
	else
	{
		updatedTime = false;
	}
	return updatedTime;
}
TimeStamp SharedClockSlaveManager::getTimeReference() const
{
	return globalTimeStamp;
}
}
}

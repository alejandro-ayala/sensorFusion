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
SharedClockSlaveManager::SharedClockSlaveManager(const std::shared_ptr<TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::CanController> cancontroller) : timeController(timecontroller), canController(cancontroller)
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
	bool updatedTime;
	CanSyncMessage globalTimeSynMsg;
	CanFUPMessage  globalTimeFupMsg;
	const auto rxMsg = canController->receiveMsg();
	if(rxMsg.dlc != 0 && rxMsg.id == static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC))
	{
		if(rxMsg.data[0] == static_cast<uint8_t>(SYNC_MSG_TYPES::SYNC_MSG))
		{
			globalTimeSynMsg.type        = rxMsg.data[0];
			globalTimeSynMsg.crc         = rxMsg.data[1];
			globalTimeSynMsg.secCounter  = rxMsg.data[2];
			globalTimeSynMsg.userByte    = rxMsg.data[3];
			globalTimeSynMsg.syncTimeSec = rxMsg.data[4] << 24 | rxMsg.data[5] << 16 | rxMsg.data[6] << 8 | rxMsg.data[7];
//			xil_printf(
//					"SYNC -- type: %02x, crc: %02x, cnt: %02x, userByte: %02x, sec: %02x \n",
//					globalTimeSynMsg.type, globalTimeSynMsg.crc,
//					globalTimeSynMsg.secCounter, globalTimeSynMsg.userByte,
//					globalTimeSynMsg.syncTimeSec);
			globalTimeStamp.seconds = globalTimeSynMsg.syncTimeSec;
			LOG_TRACE("globalTimeSynMsg --> type: ", globalTimeSynMsg.type, " CRC: ", globalTimeSynMsg.crc, " secCounter: ", globalTimeSynMsg.secCounter );
			LOG_TRACE("globalTimeStamp.seconds: ", globalTimeStamp.seconds);
			updatedTime = false;
		}
		else if(rxMsg.data[0] == static_cast<uint8_t>(SYNC_MSG_TYPES::FUP_MSG))
		{
			globalTimeFupMsg.type        = rxMsg.data[0];
			globalTimeFupMsg.crc         = rxMsg.data[1];
			globalTimeFupMsg.secCounter  = rxMsg.data[2];
			globalTimeFupMsg.overflowSeconds    = rxMsg.data[3];
			globalTimeFupMsg.syncTimeNSec = rxMsg.data[4] << 24 | rxMsg.data[5] << 16 | rxMsg.data[6] << 8 | rxMsg.data[7];

			globalTimeStamp.nanoseconds=globalTimeFupMsg.syncTimeNSec;
			LOG_TRACE("globalTimeFupMsg --> type: ", globalTimeFupMsg.type, " CRC: ", globalTimeFupMsg.crc, " secCounter: ", globalTimeFupMsg.secCounter );
			LOG_TRACE("globalTimeStamp.nanoseconds: ", globalTimeStamp.nanoseconds);

			uint64_t currentGT = timeController->getLocalTime();
			uint64_t newGT = (globalTimeStamp.seconds * 1e9) + globalTimeStamp.nanoseconds;
			LOG_TRACE("newGT: ", newGT,  "currentGT: ",  currentGT);
			timeController->setGlobalTimeReference(globalTimeStamp);
			updatedTime = true;
		}
		else
		{
			LOG_WARNING("Wrong CAN_IDs::CLOCK_SYNC: size = " , std::to_string(rxMsg.dlc));
			for(int i=0;i<rxMsg.dlc;i++)
			{
				LOG_WARNING(rxMsg.data[i], ",");
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

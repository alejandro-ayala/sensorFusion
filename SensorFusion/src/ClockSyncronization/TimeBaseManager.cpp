#include "ClockSyncronization/TimeBaseManager.h"
#include "ClockSyncronization/CanSyncMessages.h"
#include "CAN/CanIDs.h"

using namespace Communication;
namespace ClockSyncronization
{

TimeBaseManager::TimeBaseManager(TimeController* timecontroller,
		Communication::ICommunication* icomm,
		Conectivity::HTTPClient* httpclient) :
		timeController(timecontroller), canController(icomm), httpClient(
				httpclient), seqCounter(0) {

}

TimeBaseManager::~TimeBaseManager() {}

void TimeBaseManager::initialization()
{
	timeController->initialize();
	canController->initialize();
	syncTimeReference();
}

TimeStamp TimeBaseManager::getGlobalTime()
{
	return globalTimeStamp;
}

void TimeBaseManager::updateGlobalTime(TimeStamp newTimeStamp)
{
	globalTimeStamp = newTimeStamp;
}

void TimeBaseManager::syncTimeReference()
{
	//static const cstring url = "http://worldtimeapi.org/api/timezone/Europe/Madrid";
	//auto response = httpClient->getRequest(url);
	cstring response2 = "Wed Jul 15 09:15:00 2020";
	globalTimeReference = getReferenceTime(response2);
}

void TimeBaseManager::sendSyncMessage()
{
	CanSyncMessage syncMsg;

	globalTimeStamp.t_0_c   = timeController->getCurrentNsecTime();
	globalTimeStamp.seconds = globalTimeStamp.t_0_c/1000000000; // Get second portion
	globalTimeStamp.nanoseconds = globalTimeStamp.t_0_c - globalTimeStamp.seconds * 1000000000;  //Get ns portion
	//syncMsg.crc = 0;//TODO add CRC calculation
	syncMsg.secCounter = seqCounter;
	//syncMsg.userByte = 0;//TODO check why it is needed
	syncMsg.syncTimeSec = globalTimeStamp.seconds;
	uint8_t* serializedMsg;
	uint8_t frameSize = syncMsg.serialize(serializedMsg);
	xil_printf(
			"SYNC -- type: %02x, crc: %02x, cnt: %02x, uByte: %02x, sec: %d \n\r",
			syncMsg.type, syncMsg.crc,
			syncMsg.secCounter, syncMsg.userByte,
			syncMsg.syncTimeSec);
	canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC), serializedMsg,frameSize);
	globalTimeStamp.tx_stamp = timeController->getCurrentNsecTime();
}

void TimeBaseManager::sendFollowUpMessage()
{
	CanFUPMessage fupMsg;
	uint32_t t_tx  = globalTimeStamp.nanoseconds + (globalTimeStamp.tx_stamp - globalTimeStamp.t_0_c);
	//fupMsg.crc = 0;//TODO add CRC calculation --> maybe in the serialize method?
	fupMsg.secCounter = seqCounter;
	if(t_tx > 1000000000)
		fupMsg.overflowSeconds = 1;
	else
		fupMsg.overflowSeconds = 0;

	fupMsg.syncTimeNSec = t_tx;

	uint8_t* serializedMsg;
	uint8_t frameSize = fupMsg.serialize(serializedMsg);
	xil_printf(
			"FUP -- type: %02x, crc: %02x, cnt: %02x, ovf: %02x, nsec: %d \n\r",
			fupMsg.type, fupMsg.crc,
			fupMsg.secCounter, fupMsg.overflowSeconds,
			fupMsg.syncTimeNSec);

	canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC), serializedMsg,frameSize);
}

void TimeBaseManager::sendGlobalTime()
{
	sendSyncMessage();

	sendFollowUpMessage();
	seqCounter++;
}

TimeBaseRef TimeBaseManager::getReferenceTime(cstring response)
{
	//TODO parse response to TimeBaseRef
	TimeBaseRef timeRef;
	timeRef.unixTime = 1673181202; //seconds
	timeRef.utcDateTime = "2023-01-08T12:33:22+00:00";
	timeRef.year = 2023;
	timeRef.month = 1;
	timeRef.day = 8;
	timeRef.hour = 12;
	timeRef.min = 33;
	timeRef.sec = 22;
	return timeRef;
}

}

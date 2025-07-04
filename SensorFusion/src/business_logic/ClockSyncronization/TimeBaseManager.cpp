#include <business_logic/ClockSyncronization/CanSyncMessages.h>
#include <business_logic/ClockSyncronization/TimeBaseManager.h>
#include <business_logic/Communication/CanIDs.h>
#include "Logger/LoggerMacros.h"
#include <ctime>
#include <cstdio>

namespace business_logic
{
using namespace hardware_abstraction::Controllers;

using namespace Communication;
namespace ClockSyncronization
{
TimeBaseManager::TimeBaseManager(const std::shared_ptr<TimeController>& timecontroller, const std::shared_ptr<hardware_abstraction::Controllers::PsCanController>& icomm/*, const std::shared_ptr<Conectivity::HTTPClient>& httpclient*/) :
		timeController(timecontroller), canController(icomm), /*httpClient(
				httpclient),*/ seqCounter(0) {

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
	cstring response2 = "Thu Mar 06 22:34:41 2025";
	globalTimeReference = parseReferenceTime(response2);
}

uint64_t TimeBaseManager::getAbsotuleTime() const
{
	return globalTimeReference.toNs() + timeController->getCurrentNsecTime();
}

uint64_t TimeBaseManager::getRelativeTime() const
{
	return globalTimeStamp.toNs() + timeController->getCurrentNsecTime();
}

bool TimeBaseManager::sendSyncMessage()
{
	CanSyncMessage syncMsg;

	globalTimeStamp.t_0_c = getAbsotuleTime();
	globalTimeStamp.seconds = globalTimeStamp.t_0_c/1000000000; // Get second portion
	globalTimeStamp.nanoseconds = globalTimeStamp.t_0_c - globalTimeStamp.seconds * 1000000000;  //Get ns portion
	//syncMsg.crc = 0;//TODO add CRC calculation
	syncMsg.secCounter = seqCounter;
	//syncMsg.userByte = 0;//TODO check why it is needed
	syncMsg.syncTimeSec = globalTimeStamp.seconds;
	uint8_t serializedMsg[8];

	uint8_t frameSize = syncMsg.serialize(serializedMsg);

	auto result = canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC), serializedMsg,frameSize);
	if(result)
	{
		globalTimeStamp.tx_stamp = timeController->getCurrentNsecTime();
		LOG_TRACE("TimeBaseManager::sendSyncMessage -- sec:",  std::to_string(syncMsg.syncTimeSec) );
	}

	return result;
}

bool TimeBaseManager::sendFollowUpMessage()
{
	CanFUPMessage fupMsg;
	uint32_t sentNs  = globalTimeStamp.nanoseconds + (globalTimeStamp.tx_stamp - globalTimeStamp.t_0_c);
	globalTimeStamp.nanoseconds = sentNs;
	//fupMsg.crc = 0;//TODO add CRC calculation --> maybe in the serialize method?
	fupMsg.secCounter = seqCounter;
	if(sentNs > 1000000000)
		fupMsg.overflowSeconds = 1;
	else
		fupMsg.overflowSeconds = 0;

	fupMsg.syncTimeNSec = sentNs;

	uint8_t serializedMsg[8];
	uint8_t frameSize = fupMsg.serialize(serializedMsg);

	auto result = canController->transmitMsg(static_cast<uint8_t>(CAN_IDs::CLOCK_SYNC), serializedMsg,frameSize);
	if(result)
		LOG_TRACE("TimeBaseManager::sendFollowUpMessage -- nsec:", std::to_string(fupMsg.syncTimeNSec));
	return result;
}

void TimeBaseManager::sendGlobalTime()
{
	sendSyncMessage();
	sendFollowUpMessage();
	seqCounter++;
}


TimeBaseRef TimeBaseManager::parseReferenceTime(const std::string& response) {
    struct std::tm tm = {};
    char weekDay[4], month[4];

    // Parsea la fecha con sscanf (formato: "Wed Jul 15 09:15:00 2020")
    if (sscanf(response.c_str(), "%3s %3s %d %d:%d:%d %d",
               weekDay, month, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec, &tm.tm_year) != 7) {
        throw std::runtime_error("Error parsing date");
    }

    // Convertir el mes de texto a número
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; ++i) {
        if (std::string(month) == months[i]) {
            tm.tm_mon = i;
            break;
        }
    }

    // Ajustar año para tm struct (años desde 1900)
    tm.tm_year -= 1900;
    tm.tm_isdst = -1;  // Dejar que el sistema determine si hay horario de verano

    // Convertir a tiempo UNIX
    time_t unixTime = std::mktime(&tm);

    // Formatear la fecha en formato ISO 8601
    char isoDate[30];
    std::strftime(isoDate, sizeof(isoDate), "%Y-%m-%dT%H:%M:%S%z", &tm);

    // Construir el objeto TimeBaseRef
    TimeBaseRef timeRef;
    timeRef.unixTime = unixTime;
    timeRef.utcDateTime = isoDate;
    timeRef.year = tm.tm_year + 1900;
    timeRef.month = tm.tm_mon + 1;
    timeRef.day = tm.tm_mday;
    timeRef.hour = tm.tm_hour;
    timeRef.min = tm.tm_min;
    timeRef.sec = tm.tm_sec;

    return timeRef;
}
}
}

#pragma once
namespace ClockSyncronization
{
enum class SYNC_MSG_TYPES
{
	SYNC_MSG = 0x20,
	FUP_MSG  = 0x28
};

typedef struct
{
	uint8_t type = static_cast<uint8_t>(SYNC_MSG_TYPES::SYNC_MSG);
	uint8_t crc = 0xFE;
	uint8_t secCounter; //In autosar: seqCounter[0:3] and timedoman[4:7]
	uint8_t userByte = 0x13;
	uint32_t syncTimeSec;
	//uint8 reserved[8];

	uint8_t serialize(uint8_t* bufferPtr)
	{
		uint8_t buffer[8] = {type,crc,secCounter,userByte,(syncTimeSec & 0xFF000000) >> 24 , (syncTimeSec & 0x00FF0000) >> 16 , (syncTimeSec & 0x0000FF00) >> 8, syncTimeSec & 0x000000FF};
		for(int i = 0; i<8; i++)
		{
			bufferPtr[i] = buffer[i];
		}

		return 8;
	}
} CanSyncMessage;

typedef struct
{
	uint8_t type = static_cast<uint8_t>(SYNC_MSG_TYPES::FUP_MSG);
	uint8_t crc;
	uint8_t secCounter; //In autosar: seqCounter[0:3] and timedoman[4:7]
	uint8_t overflowSeconds; //In autosar: reserved(bit 7 to 3) SGW(bit2), OVS(bit1 to 0)
	uint32_t syncTimeNSec;
	//uint8 reserved[8];

	uint8_t serialize(uint8_t* bufferPtr)
	{
		uint8_t buffer[8] = {type,crc,secCounter,overflowSeconds,syncTimeNSec & 0xFF000000, syncTimeNSec & 0x00FF0000 , syncTimeNSec & 0x0000FF00, syncTimeNSec & 0x000000FF};
		bufferPtr = buffer;
		return 8;
	}
} CanFUPMessage;
}

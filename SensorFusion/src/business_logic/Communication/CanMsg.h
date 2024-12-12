#pragma once
#include "CanIDs.h"

namespace business_logic
{
namespace Communication
{

#define MAXIMUM_CAN_MSG_SIZE 8
#define ID_FIELD_SIZE 3

typedef struct CanMsg
{
	uint8_t canMsgId;
	uint8_t lsbCanMsgIndex;
	uint8_t msbCanMsgIndex;
	uint8_t payload[MAXIMUM_CAN_MSG_SIZE - ID_FIELD_SIZE];
	uint8_t payloadSize;
} CanMsg;
}
}

#pragma once

namespace business_logic
{
namespace Communication
{
struct IData {

	uint8_t serialize(uint8_t* bufferPtr)
	{
		uint8_t buffer[] = {typeMsg,data[0],data[1],data[2],data[3],sizeData,secCounter,(timestamp & 0xFF000000) >> 24 , (timestamp & 0x00FF0000) >> 16 , (timestamp & 0x0000FF00) >> 8, timestamp & 0x000000FF,crc};
		for(int i = 0; i<(sizeof(buffer)/sizeof(buffer[0])); i++)
		{
			bufferPtr[i] = buffer[i];
		}

		return 5;
	}

	void deSerialize(uint8_t* bufferPtr)
	{
		typeMsg    = bufferPtr[0];
		sizeData   = bufferPtr[5];
		timestamp  = (bufferPtr[7] << 24) | (bufferPtr[8] << 16) | (bufferPtr[9] << 8) | (bufferPtr[10]);
		secCounter = bufferPtr[6];
		crc        = bufferPtr[11];
		for(int i = 0; i < sizeData ; i++)
			data[i] = bufferPtr[1+i];
	}
	uint8_t typeMsg;
	uint8_t data[20];
	uint8_t sizeData;
	uint64_t timestamp;
	uint8_t secCounter;
	uint8_t crc;
};
}
}

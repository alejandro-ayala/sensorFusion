#pragma once

typedef struct
{
	cstring  utcDateTime;
	uint16_t year;
	uint16_t month;
	uint16_t day;
	uint16_t hour;
	uint16_t min;
	uint16_t sec;
	uint32_t unixTime;
} TimeBaseRef;

typedef struct
{
	uint32_t nanoseconds;
	uint32_t seconds;
	uint16_t secondsHi;
	uint64_t tx_stamp;
	uint64_t t_0_c;
} TimeStamp;

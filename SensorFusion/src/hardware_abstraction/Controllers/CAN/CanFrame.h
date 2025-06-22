#pragma once
#include <array>
#include <map>
namespace hardware_abstraction
{
namespace Controllers
{
constexpr uint8_t CAN_DATA_PAYLOAD_SIZE = 8;

enum class CanRxBuffer {
   CanRx0 = 0, CanRx1, None
};

enum class CanTxBuffer {
   CanTx0 = 0, CanTx1, CanTx2
};

enum class CanMode {
   NormalOperation = 0,
   Sleep,
   Loopback,
   ListenOnly,
   Configuration = 0x80
};

enum class CanPsMode {
   NormalOperation = 0,
   Sleep,
   Loopback,
   ListenOnly,
   Config,
   Snoop
};

struct CanTimingParams{
    uint8_t brpr;   // Baud Rate Prescaler Register
    uint8_t tseg1;  // Time Segment 1
    uint8_t tseg2;  // Time Segment 2
    uint8_t sjw;    // Synchronization Jump Width
};
//Xil_AssertNonvoid(TimeSegment1 <= (u8)15U );
//Xil_AssertNonvoid(TimeSegment2 <= (u8)7U);
//Xil_AssertNonvoid(SyncJumpWidth <= (u8)3U);

enum class CanBusBaudrates
{
	_40kbps, _250kbps, _500kbps, _1Mbps
};

static std::map<CanBusBaudrates, CanTimingParams> canTimingPresets = {
    {CanBusBaudrates::_40kbps,  {6, 12, 3, 3}},   // 24 MHz clock
    {CanBusBaudrates::_250kbps, {5, 11, 2, 2}},    // 24 MHz clock
    {CanBusBaudrates::_500kbps, {2, 8, 5, 3}},    // 24 MHz clock
    {CanBusBaudrates::_1Mbps,   {1, 5, 4, 2}},    // 24 MHz clock
};

struct CanFrame
{
   uint16_t id;     // 11 bit id
   uint32_t eid;    // 18 bit extended id
   uint8_t ide;     // 1 to enable sending extended id
   uint8_t rtr;     // Remote transmission request bit
   uint8_t srr;     // Standard Frame Remote Transmit Request
   uint8_t dlc;     // Data length
   std::array<uint8_t, CAN_DATA_PAYLOAD_SIZE> data; // Data buffer
   // Some additional information has not yet been encapsulated here
   // (ex:priority bits), primarily, no TXBxCTRL bits
};

}
}

#pragma once
#include <array>
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

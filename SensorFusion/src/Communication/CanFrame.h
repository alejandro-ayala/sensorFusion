

namespace Communication
{

typedef enum CAN_RxBuffer {
   CAN_Rx0 = 0, CAN_Rx1
} CAN_RxBuffer;

typedef enum CAN_TxBuffer {
   CAN_Tx0 = 0, CAN_Tx1, CAN_Tx2
} CAN_TxBuffer;

typedef enum CAN_Mode {
   CAN_ModeNormalOperation = 0,
   CAN_ModeSleep,
   CAN_ModeLoopback,
   CAN_ModeListenOnly,
   CAN_ModeConfiguration = 0x80
} CAN_Mode;

typedef struct CanFrame {
   uint16_t id;     // 11 bit id
   uint32_t eid;    // 18 bit extended id
   uint8_t ide;     // 1 to enable sending extended id
   uint8_t rtr;     // Remote transmission request bit
   uint8_t srr;     // Standard Frame Remote Transmit Request
   uint8_t dlc;     // Data length
   uint8_t data[8]; // Data buffer
   // Some additional information has not yet been encapsulated here
   // (ex:priority bits), primarily, no TXBxCTRL bits
} CanFrame;

}  // namespace Communication

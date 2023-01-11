#pragma once

namespace Communication
{
#define CAN_MODIFY_REG_CMD        0x05

#define CAN_WRITE_REG_CMD         0x02

#define CAN_READ_REG_CMD          0x03

#define CAN_RTS_CMD               0x80
#define CAN_RTS_TXB0_MASK         0x01
#define CAN_RTS_TXB1_MASK         0x02
#define CAN_RTS_TXB2_MASK         0x04

#define CAN_LOADBUF_CMD           0x40
#define CAN_LOADBUF_TXB0SIDH      0x00
#define CAN_LOADBUF_TXB0D0        0x01
#define CAN_LOADBUF_TXB1SIDH      0x02
#define CAN_LOADBUF_TXB1D0        0x03
#define CAN_LOADBUF_TXB2SIDH      0x04
#define CAN_LOADBUF_TXB2D0        0x05

#define CAN_READBUF_CMD           0x90
#define CAN_READBUF_RXB0SIDH      0x00
#define CAN_READBUF_RXB0D0        0x01
#define CAN_READBUF_RXB1SIDH      0x02
#define CAN_READBUF_RXB1D0        0x03

#define CAN_READSTATUS_CMD        0xA0
#define CAN_STATUS_RX0IF_MASK     0x01
#define CAN_STATUS_RX1IF_MASK     0x02
#define CAN_STATUS_TX0REQ_MASK    0x04
#define CAN_STATUS_TX0IF_MASK     0x08
#define CAN_STATUS_TX1REQ_MASK    0x10
#define CAN_STATUS_TX1IF_MASK     0x20
#define CAN_STATUS_TX2REQ_MASK    0x40
#define CAN_STATUS_TX2IF_MASK     0x80

#define CAN_RXSTATUS_CMD          0xB0
#define CAN_RXSTATUS_RX0IF_MASK   0x40
#define CAN_RXSTATUS_RX1IF_MASK   0x80

#define CAN_CANCTRL_REG_ADDR      0x0F
#define CAN_CNF3_REG_ADDR         0x28
#define CAN_CNF2_REG_ADDR         0x29
#define CAN_CNF1_REG_ADDR         0x2A
#define CAN_RXB0CTRL_REG_ADDR     0x60

#define CAN_TXB0CTRL_REG_ADDR     0x30
#define CAN_TXB0CTRL_TXREQ_MASK   0x08

#define CAN_CANINTF_REG_ADDR      0x2C
#define CAN_CANINTF_RX0IF_MASK    0x01
#define CAN_CANINTF_RX1IF_MASK    0x02
#define CAN_CANINTF_TX0IF_MASK    0x04
#define CAN_CANINTF_TX1IF_MASK    0x08
#define CAN_CANINTF_TX2IF_MASK    0x10
#define CAN_CAN_CANCTRL_MODE_MASK 0xE0
#define CAN_CANCTRL_MODE_BIT      5


}  // namespace Communication

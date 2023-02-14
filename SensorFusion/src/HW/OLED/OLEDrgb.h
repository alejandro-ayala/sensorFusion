#pragma once
#include <cstdint>

namespace Hardware
{
constexpr uint8_t CHARBYTES  =  8;    // Number of bytes in a glyph
constexpr uint8_t USERCHAR_MAX = 0x20; // Number of character defs in user font
constexpr uint8_t OLEDRGB_WIDTH  = 96;
constexpr uint8_t OLEDRGB_HEIGHT = 64;
constexpr int CHARBYTES_USER = (USERCHAR_MAX*CHARBYTES);
class SPIController;

static uint8_t rgbUserFont[] = {
   0x00, 0x04, 0x02, 0x1F, 0x02, 0x04, 0x00, 0x00, // 0x00
   0x0E, 0x1F, 0x15, 0x1F, 0x17, 0x10, 0x1F, 0x0E, // 0x01
   0x00, 0x1F, 0x11, 0x00, 0x00, 0x11, 0x1F, 0x00, // 0x02
   0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00, // 0x03
   0x07, 0x0C, 0xFA, 0x2F, 0x2F, 0xFA, 0x0C, 0x07  // 0x04
}; // This table defines 5 user characters, although only one is used
typedef struct
{
   uint32_t GPIO_addr;
   //SPIController OLEDSpi;

   uint8_t *pbOledrgbFontCur;
   uint8_t *pbOledrgbFontUser;
   uint8_t rgbOledrgbFontUser[CHARBYTES_USER];
   int dxcoOledrgbFontCur;
   int dycoOledrgbFontCur;
   uint16_t m_FontColor, m_FontBkColor;
   int xchOledCur;
   int ychOledCur;

   int xchOledrgbMax;
   int ychOledrgbMax;
} PmodOLEDrgb;
}

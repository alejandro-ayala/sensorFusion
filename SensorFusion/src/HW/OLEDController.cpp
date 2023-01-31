#include "xil_types.h"
#include "xspi.h"
#include "xspi_l.h"
#include "xstatus.h"
#include "sleep.h"

#include "OLEDController.h"
#include "OLEDcommands.h"
#include "OLEDrgbFont.h"
#include "Common/bitmap.h"
#include "SPIConfig.h"
namespace Hardware
{

OLEDController::OLEDController(): IController("OLEDController")
{
	static SPIConfig xspiCfg;
	xspiCfg.spiConfiguration = {0,0,1,0,1,8,0,0,0,0,0};
	xspiCfg.gpioAddress      = XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR;
	xspiCfg.spiBaseAddress   = XPAR_PMODOLEDRGB_0_AXI_LITE_SPI_BASEADDR;
	spiControl = new SPIController(xspiCfg);

}

OLEDController::~OLEDController()
{
	delete spiControl;
}


void OLEDController::initialize()
{
	int ib;
	oledControl.GPIO_addr = XPAR_PMODOLEDRGB_0_AXI_LITE_GPIO_BASEADDR;


	oledControl.dxcoOledrgbFontCur = CHARBYTES;
	oledControl.dycoOledrgbFontCur = 8;

	for (ib = 0; ib < CHARBYTES_USER; ib++)
	{
		oledControl.rgbOledrgbFontUser[ib] = 0;
	}
	oledControl.xchOledrgbMax = OLEDRGB_WIDTH  / oledControl.dxcoOledrgbFontCur;
	oledControl.ychOledrgbMax = OLEDRGB_HEIGHT / oledControl.dycoOledrgbFontCur;

	setFontColor(buildRGB(0, 0xFF, 0)); // Green
	setFontBkColor(buildRGB(0, 0, 0));  // Black

	setCurrentFontTable((uint8_t*) rgbOledRgbFont0);
	setCurrentUserFontTable(oledControl.rgbOledrgbFontUser);

	spiControl->initialize();

}

void OLEDController::drawPixel(uint8_t c, uint8_t r, uint16_t pixelColor)
{
	uint8_t cmds[6];
	uint8_t data[2];
	// Set column start and end
	cmds[0] = CMD_SETCOLUMNADDRESS;
	cmds[1] = c;                 // Set the starting column coordinates
	cmds[2] = OLEDRGB_WIDTH - 1; // Set the finishing column coordinates

	// Set row start and end
	cmds[3] = CMD_SETROWADDRESS;
	cmds[4] = r;                  // Set the starting row coordinates
	cmds[5] = OLEDRGB_HEIGHT - 1; // Set the finishing row coordinates

	data[0] = pixelColor >> 8;
	data[1] = pixelColor;

	spiControl->writeData(cmds, 6, data, 2);
}

void OLEDController::drawLine(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor)
{
	uint8_t cmds[8];
	cmds[0] = CMD_DRAWLINE; // Draw line
	cmds[1] = c1;           // Start column
	cmds[2] = r1;           // Start row
	cmds[3] = c2;           // End column
	cmds[4] = r2;           // End row
	cmds[5] = extractRFromRGB(lineColor);   // R
	cmds[6] = extractGFromRGB(lineColor);   // G
	cmds[7] = extractBFromRGB(lineColor);   // B

	spiControl->writeData(cmds, 8, NULL, 0);
}

void OLEDController::drawRectangle(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2,uint16_t lineColor, uint8_t bFill, uint16_t fillColor)
{
	uint8_t cmds[13];
	cmds[0] = CMD_FILLWINDOW;                       // Fill window
	cmds[1] = (bFill ? ENABLE_FILL : DISABLE_FILL);
	cmds[2] = CMD_DRAWRECTANGLE;                    // Draw rectangle
	cmds[3] = c1;                                   // Start column
	cmds[4] = r1;                                   // Start row
	cmds[5] = c2;                                   // End column
	cmds[6] = r2;                                   // End row

	cmds[7] = extractRFromRGB(lineColor);   // R
	cmds[8] = extractGFromRGB(lineColor);   // G
	cmds[9] = extractBFromRGB(lineColor);   // B

	cmds[10] = extractRFromRGB(fillColor);  // R
	cmds[11] = extractGFromRGB(fillColor);  // G
	cmds[12] = extractBFromRGB(fillColor);  // B

	spiControl->writeData(cmds, 13, NULL, 0);
}

void OLEDController::clear()
{
	uint8_t cmds[5];
	cmds[0] = CMD_CLEARWINDOW;     // Enter the "clear mode"
	cmds[1] = 0x00;                // Set the starting column coordinates
	cmds[2] = 0x00;                // Set the starting row coordinates
	cmds[3] = OLEDRGB_WIDTH - 1;   // Set the finishing column coordinates;
	cmds[4] = OLEDRGB_HEIGHT - 1;  // Set the finishing row coordinates;
	usleep(1000 * 5);

	spiControl->writeData(cmds, 5, NULL, 0);
}

void OLEDController::drawBitmap(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t *pBmp)
{
	uint8_t cmds[6];
	//set column start and end
	cmds[0] = CMD_SETCOLUMNADDRESS;
	cmds[1] = c1;                   // Set the starting column coordinates
	cmds[2] = c2;                   // Set the finishing column coordinates

	//set row start and end
	cmds[3] = CMD_SETROWADDRESS;
	cmds[4] = r1;                   // Set the starting row coordinates
	cmds[5] = r2;                   // Set the finishing row coordinates

	usleep(1000 * 5);
	spiControl->writeData( cmds, 6, pBmp,(((c2 - c1 + 1) * (r2 - r1 + 1)) << 1));
}


void OLEDController::setCursor(int xch, int ych)
{
	// Clamp the specified location to the display surface
	if (xch >= oledControl.xchOledrgbMax)
	{
	  xch = oledControl.xchOledrgbMax - 1;
	}

	if (ych >= oledControl.ychOledrgbMax)
	{
	  ych = oledControl.ychOledrgbMax - 1;
	}

	// Save the given character location.
	oledControl.xchOledCur = xch;
	oledControl.ychOledCur = ych;
}

void OLEDController::getCursor(int *pxch, int *pych)
{
	*pxch = oledControl.xchOledCur;
	*pych = oledControl.ychOledCur;
}

int  OLEDController::defUserChar(char ch, uint8_t *pbDef)
{
	uint8_t *pb;
	int ib;

	if (ch < USERCHAR_MAX)
	{
	  pb = oledControl.pbOledrgbFontUser + ch * CHARBYTES;
	  for (ib = 0; ib < CHARBYTES; ib++)
	  {
		 *pb++ = *pbDef++;
	  }
	  return 1;
	}
	else
	{
	  return 0;
	}
}

void OLEDController::drawGlyph(char ch)
{
	uint8_t *pbFont;
	int ibx, iby, iw, x, y;
	u16 rgwCharBmp[CHARBYTES << 4];

	if ((ch & 0x80) != 0)
	{
		return;
	}

	if (ch < USERCHAR_MAX)
	{
		pbFont = oledControl.pbOledrgbFontUser + ch * CHARBYTES;
	}
	else if ((ch & 0x80) == 0)
	{
		pbFont = oledControl.pbOledrgbFontCur + (ch - USERCHAR_MAX) * CHARBYTES;
	}

	iw = 0;
	for (iby = 0; iby < oledControl.dycoOledrgbFontCur; iby++)
	{
		for (ibx = 0; ibx < oledControl.dxcoOledrgbFontCur; ibx++)
		{
			if (pbFont[ibx] & (1 << iby))
			{
			// Point in glyph
				rgwCharBmp[iw] = oledControl.m_FontColor;
			}
			else
			{
				// Background
				rgwCharBmp[iw] = oledControl.m_FontBkColor;
			}
			iw++;
		}
	}
	x = oledControl.xchOledCur * oledControl.dxcoOledrgbFontCur;
	y = oledControl.ychOledCur * oledControl.dycoOledrgbFontCur;

	drawBitmap(x, y, x + CHARBYTES - 1, y + 7, (uint8_t*) rgwCharBmp);
}

void OLEDController::putChar(char ch)
{
	drawGlyph(ch);
	advanceCursor();
}

void OLEDController::putString(char *sz)
{
	while (*sz != '\0')
	{
		drawGlyph(*sz);
		advanceCursor();
		sz += 1;
	}
}

void OLEDController::setFontColor(uint16_t fontColor)
{
	oledControl.m_FontColor = fontColor;
}

void OLEDController::setFontBkColor(uint16_t fontBkColor)
{
	oledControl.m_FontBkColor = fontBkColor;
}

void OLEDController::setCurrentFontTable(uint8_t *pbFont)
{
	oledControl.pbOledrgbFontCur = pbFont;
}

void OLEDController::setCurrentUserFontTable(uint8_t *pbUserFont)
{
	oledControl.pbOledrgbFontUser = pbUserFont;
}

void OLEDController::advanceCursor()
{
	oledControl.xchOledCur += 1;
	if (oledControl.xchOledCur >= oledControl.xchOledrgbMax)
	{
		oledControl.xchOledCur = 0;
		oledControl.ychOledCur += 1;
	}
	if (oledControl.ychOledCur >= oledControl.ychOledrgbMax)
	{
		oledControl.ychOledCur = 0;
	}
	setCursor(oledControl.xchOledCur, oledControl.ychOledCur);
}

void OLEDController::setScrolling(uint8_t scrollH, uint8_t scrollV,uint8_t rowAddr, uint8_t rowNum, uint8_t timeInterval)
{
	uint8_t cmds[7];
	cmds[0] = CMD_CONTINUOUSSCROLLINGSETUP;
	cmds[1] = scrollH;             // Horizontal scroll
	cmds[2] = rowAddr;             // Start row address
	cmds[3] = rowNum;              // Number of scrolling rows
	cmds[4] = scrollV;             // Vertical scroll
	cmds[5] = timeInterval;        // Time interval
	cmds[6] = CMD_ACTIVESCROLLING; // Set the starting row coordinates

	usleep(1000 * 5);
	spiControl->writeData(cmds, 7, NULL, 0);
}

void OLEDController::enableScrolling(uint8_t fEnable)
{
	spiControl->writeCommand(fEnable ? CMD_ACTIVESCROLLING : CMD_DEACTIVESCROLLING);
}

void OLEDController::enableBackLight(uint8_t fEnable)
{
   if (fEnable)
   {
		Xil_Out32(oledControl.GPIO_addr + 4, Xil_In32(oledControl.GPIO_addr + 4) & 0xB); // 0b1011
		Xil_Out32(oledControl.GPIO_addr, Xil_In32(oledControl.GPIO_addr) | 0x4); // 0b0100
		usleep(1000 * 25);
		//writeSPICommand(CMD_DISPLAYON);
		spiControl->writeCommand(CMD_DISPLAYON);
   }
   else
   {
		//writeSPICommand(CMD_DISPLAYOFF);
		spiControl->writeCommand(CMD_DISPLAYOFF);

		Xil_Out32(oledControl.GPIO_addr, Xil_In32(oledControl.GPIO_addr) & 0xB); // 0b1011
		Xil_Out32(oledControl.GPIO_addr + 4, Xil_In32(oledControl.GPIO_addr + 4) | 0x4); // 0b0100
   }
}

void OLEDController::copy(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t c3, uint8_t r3)
{
	uint8_t cmds[7];
	cmds[0] = CMD_COPYWINDOW;
	cmds[1] = c1;                   // Set the starting column coordinates
	cmds[2] = r1;                   // Set the starting row coordinates
	cmds[3] = c2;                   // Set the finishing column coordinates
	cmds[4] = r2;                   // Set the finishing row coordinates
	cmds[5] = c3;                   // Set the new starting column coordinates
	cmds[6] = r3;                   // Set the new starting row coordinates

	usleep(1000 * 5);
	spiControl->writeData(cmds, 7, NULL, 0);
}

void OLEDController::dim(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2)
{
	uint8_t cmds[5];
	cmds[0] = CMD_DIMWINDOW;
	cmds[1] = c1; // Set the starting column coordinates
	cmds[2] = r1; // Set the starting row coordinates
	cmds[3] = c2; // Set the finishing column coordinates
	cmds[4] = r2; // Set the finishing row coordinates

	usleep(1000 * 5);
	spiControl->writeData(cmds, 5, NULL, 0);
}

void OLEDController::screenOn()
{
	uint8_t cmds[39];
	Xil_Out32(oledControl.GPIO_addr + 4, 0x0000);
	Xil_Out32(oledControl.GPIO_addr, 0xA);

	// Bring PmodEn HIGH
	Xil_Out32(oledControl.GPIO_addr, 0xA); // 0b1010
	usleep(1000 * 20); // Delay for 20us

	// Assert Reset
	Xil_Out32(oledControl.GPIO_addr, 0x8); // 0b1000
	usleep(1000 * 1);
	Xil_Out32(oledControl.GPIO_addr, 0xA); // 0b1010
	usleep(1000 * 2);

	// Command un-lock
	cmds[0] = 0xFD;
	cmds[1] = 0x12;
	// 5. Univision Initialization Steps
	// 5a) Set Display Off
	cmds[2] = CMD_DISPLAYOFF;
	// 5b) Set Remap and Data Format
	cmds[3] = CMD_SETREMAP;
	cmds[4] = 0x72;
	// 5c) Set Display Start Line
	cmds[5] = CMD_SETDISPLAYSTARTLINE;
	cmds[6] = 0x00; // Start line is set at upper left corner
	// 5d) Set Display Offset
	cmds[7] = CMD_SETDISPLAYOFFSET;
	cmds[8] = 0x00; //no offset
	// 5e)
	cmds[9] = CMD_NORMALDISPLAY;
	// 5f) Set Multiplex Ratio
	cmds[10] = CMD_SETMULTIPLEXRATIO;
	cmds[11] = 0x3F; //64MUX
	// 5g)Set Master Configuration
	cmds[12] = CMD_SETMASTERCONFIGURE;
	cmds[13] = 0x8E;
	// 5h)Set Power Saving Mode
	cmds[14] = CMD_POWERSAVEMODE;
	cmds[15] = 0x0B;
	// 5i) Set Phase Length
	cmds[16] = CMD_PHASEPERIODADJUSTMENT;
	cmds[17] = 0x31; // Phase 2 = 14 DCLKs, phase 1 = 15 DCLKS
	// 5j) Send Clock Divide Ratio and Oscillator Frequency
	cmds[18] = CMD_DISPLAYCLOCKDIV;
	cmds[19] = 0xF0; // Mid high oscillator frequency, DCLK = FpbCllk/2
	// 5k) Set Second Pre-charge Speed of Color A
	cmds[20] = CMD_SETPRECHARGESPEEDA;
	cmds[21] = 0x64;
	// 5l) Set Set Second Pre-charge Speed of Color B
	cmds[22] = CMD_SETPRECHARGESPEEDB;
	cmds[23] = 0x78;
	// 5m) Set Second Pre-charge Speed of Color C
	cmds[24] = CMD_SETPRECHARGESPEEDC;
	cmds[25] = 0x64;
	// 5n) Set Pre-Charge Voltage
	cmds[26] = CMD_SETPRECHARGEVOLTAGE;
	cmds[27] = 0x3A; // Pre-charge voltage =...Vcc
	// 50) Set VCOMH Deselect Level
	cmds[28] = CMD_SETVVOLTAGE;
	cmds[29] = 0x3E; // Vcomh = ...*Vcc
	// 5p) Set Master Current
	cmds[30] = CMD_MASTERCURRENTCONTROL;
	cmds[31] = 0x06;
	// 5q) Set Contrast for Color A
	cmds[32] = CMD_SETCONTRASTA;
	cmds[33] = 0x91;
	// 5r) Set Contrast for Color B
	cmds[34] = CMD_SETCONTRASTB;
	cmds[35] = 0x50;
	// 5s) Set Contrast for Color C
	cmds[36] = CMD_SETCONTRASTC;
	cmds[37] = 0x7D;
	// Disable scrolling
	cmds[38] = CMD_DEACTIVESCROLLING;

	spiControl->writeData(cmds, 39, NULL, 0);

	// 5u) Clear Screen
	clear();
	// Turn on VCC and wait for it to become stable
	Xil_Out32(oledControl.GPIO_addr, 0b1110);

	usleep(1000 * 25);

	// Send Display On command
	spiControl->writeCommand(CMD_DISPLAYON);

	usleep(1000 * 100);
}

void OLEDController::screenOff()
{
	spiControl->writeCommand(CMD_DISPLAYOFF);
	Xil_Out32(oledControl.GPIO_addr, Xil_In32(oledControl.GPIO_addr) & 0xB);
	usleep(1000 * 400);

	spiControl->finish();
	// Make signal pins and power control pins be inputs.
	Xil_Out32(oledControl.GPIO_addr, 0x3); // 0b0011
	Xil_Out32(oledControl.GPIO_addr + 4, 0xF); // 0b1111
}


uint16_t OLEDController::buildHSV(uint8_t hue, uint8_t sat, uint8_t val)
{
	uint8_t region, remain, p, q, t;
	uint8_t R, G, B;
	region = hue / 43;
	remain = (hue - (region * 43)) * 6;
	p = (val * (255 - sat)) >> 8;
	q = (val * (255 - ((sat * remain) >> 8))) >> 8;
	t = (val * (255 - ((sat * (255 - remain)) >> 8))) >> 8;

	switch (region)
	{
	case 0:
	  R = val;
	  G = t;
	  B = p;
	  break;
	case 1:
	  R = q;
	  G = val;
	  B = p;
	  break;
	case 2:
	  R = p;
	  G = val;
	  B = t;
	  break;
	case 3:
	  R = p;
	  G = q;
	  B = val;
	  break;
	case 4:
	  R = t;
	  G = p;
	  B = val;
	  break;
	default:
	  R = val;
	  G = p;
	  B = q;
	  break;
	}
	return buildRGB(R, G, B);
}

uint16_t OLEDController::buildRGB(uint8_t R, uint8_t G, uint8_t B)
{
	return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
}


uint8_t OLEDController::extractRFromRGB(uint16_t wRGB)
{
	return (uint8_t) ((wRGB >> 11) & 0x1F);
}

uint8_t OLEDController::extractGFromRGB(uint16_t wRGB)
{
	return (uint8_t) ((wRGB >> 5) & 0x3F);
}

uint8_t OLEDController::extractBFromRGB(uint16_t wRGB)
{
	return (uint8_t) (wRGB & 0x1F);
}

bool OLEDController::selfTest()
{
	initialize();

	screenOn();

	setCursor(2, 1);
	putString("Sensor fusion"); // Default color (green)
	setCursor(4, 4);
	setFontColor(buildRGB(0, 0, 255)); // Blue font
	putString("SelfTest");

	setFontColor(buildRGB(200, 200, 44));
	setCursor(1, 6);
	putChar(4);

	setFontColor(buildRGB(200, 12, 44));
	setCursor(5, 6);
	putString("Done");
	putChar(0);

	sleep(5); // Wait 5 seconds

	drawBitmap(0, 0, 95, 63, (u8*) oledBitmap);
	return true;
}
}

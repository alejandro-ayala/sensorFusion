#pragma once

#include "HW/OLEDrgb.h"
#include "IController.h"
#include "SPIController.h"
#include "TestInclude.h"

namespace Hardware
{

class OLEDController : public IController
{
private:
	SPIController* spiControl;
	PmodOLEDrgb    oledControl;
public:
	OLEDController();
	virtual ~OLEDController();

	TVIRTUAL void initialize() override;

	void drawPixel(uint8_t c, uint8_t r, uint16_t pixelColor);
	void drawLine(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint16_t lineColor);
	void drawRectangle(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2,uint16_t lineColor, uint8_t bFill, uint16_t fillColor);
	void clear();
	void drawBitmap(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t *pBmp);

	void setCursor(int xch, int ych);
	void getCursor(int *pxch, int *pych);
	int  defUserChar(char ch, uint8_t *pbDef);
	void drawGlyph(char ch);
	void putChar(char ch);
	void putString(char *sz);
	void setFontColor(uint16_t fontColor);
	void setFontBkColor(uint16_t fontBkColor);
	void setCurrentFontTable(uint8_t *pbFont);
	void setCurrentUserFontTable(uint8_t *pbUserFont);
	void advanceCursor();
	void setScrolling(uint8_t scrollH, uint8_t scrollV,uint8_t rowAddr, uint8_t rowNum, uint8_t timeInterval);
	void enableScrolling(uint8_t fEnable);
	void enablePmod(uint8_t fEnable);
	void enableBackLight(uint8_t fEnable);
	void copy(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2, uint8_t c3, uint8_t r3);
	void dim(uint8_t c1, uint8_t r1, uint8_t c2, uint8_t r2);

	void screenOn();
	void screenOff();

	uint16_t buildHSV(uint8_t hue, uint8_t sat, uint8_t val);
	uint16_t buildRGB(uint8_t R, uint8_t G, uint8_t B);

	uint8_t extractRFromRGB(uint16_t wRGB);
	uint8_t extractGFromRGB(uint16_t wRGB);
	uint8_t extractBFromRGB(uint16_t wRGB);
	bool selfTest() override;
};
}


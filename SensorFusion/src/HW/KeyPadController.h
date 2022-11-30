#pragma once
#include <unordered_map>
#include "TestInclude.h"
#include "Definitions.h"
#include "GpioInterface.h"

namespace Hardware
{
class KeyPadController
{
private:
	PmodKeyPad kypdInterface;
	std::unordered_map<uint16_t, uint8_t> keyPadMapper = { { 0xFFFF, 0x0 }, { 0x00FF,
			0x1 }, { 0x0F0F, 0x2 }, { 0x0FFF, 0x3 }, { 0x3333, 0x4 },
			{ 0x33FF, 0x5 }, { 0x3F3F, 0x6 }, { 0x033F, 0x7 }, { 0x5555, 0x8 }, {
					0x55FF, 0x9 }, { 0x5F5F, 0xA }, { 0x055F, 0xB },
			{ 0x7777, 0xC }, { 0x1177, 0xD }, { 0x1717, 0xE }, { 0x177F, 0xF }};
public:
	KeyPadController();
	virtual ~KeyPadController();

	TVIRTUAL void initialization();
	TVIRTUAL void loadKeyTable(cstring keyTable);
	TVIRTUAL void setCols(uint32_t cols);
	TVIRTUAL uint32_t getRows();
	TVIRTUAL uint16_t getKeyStates();
	TVIRTUAL uint32_t getKeyPressed(uint16_t keystate, uint8_t *cptr);
	void selfTest();
};
}

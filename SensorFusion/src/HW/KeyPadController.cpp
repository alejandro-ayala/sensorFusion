#include "KeyPadController.h"

#include "KeyPadStateEnum.h"

namespace Hardware
{
KeyPadController::KeyPadController() : IController("KeyPadController")
{}

KeyPadController::~KeyPadController() {}

void KeyPadController::initialize()
{
	kypdInterface.GPIO_addr = XPAR_PMODKYPD_0_AXI_LITE_GPIO_BASEADDR;
	// Set tri-state register, lower 4 pins are column outputs, upper 4 pins are
	// row inputs
	Xil_Out32(kypdInterface.GPIO_addr + 4, 0xF0);
	kypdInterface.keytable_loaded = FALSE;

	// keytable is determined as follows (indices shown in Keypad position below)
	// 12 13 14 15
	// 8  9  10 11
	// 4  5  6  7
	// 0  1  2  3
	constexpr char* keyTable = "0FED789C456B123A";
	loadKeyTable(keyTable);

}

void KeyPadController::loadKeyTable(cstring keyTable)
{
	for(size_t i = 0; i<16; i++)
	{
		kypdInterface.keytable[i] = keyTable[i];
	}
	kypdInterface.keytable_loaded = true;
}


void KeyPadController::setCols(uint32_t cols)
{
	Xil_Out32(kypdInterface.GPIO_addr, cols & 0xF);
}

uint32_t KeyPadController::getRows()
{
	return (Xil_In32(kypdInterface.GPIO_addr) >> 4) & 0xF;
}

uint16_t KeyPadController::getKeyStates()
{
	uint16_t shift[4] = {0, 0, 0, 0};

	// Test each column combination, this will help to detect when multiple keys
	// in the same row are pressed.
	for (uint32_t cols = 0; cols < 16; cols++)
	{
		setCols(cols);
		const uint32_t rows = getRows();
		// Group bits from each individual row
		shift[0] = (shift[0] << 1) | (rows & 0x1);
		shift[1] = (shift[1] << 1) | (rows & 0x2) >> 1;
		shift[2] = (shift[2] << 1) | (rows & 0x4) >> 2;
		shift[3] = (shift[3] << 1) | (rows & 0x8) >> 3;
	}

	// Translate shift patterns for each row into button presses.
	uint16_t keystate = 0;
	keystate |= keyPadMapper[shift[0]];
	keystate |= keyPadMapper[shift[1]] << 4;
	keystate |= keyPadMapper[shift[2]] << 8;
	keystate |= keyPadMapper[shift[3]] << 12;
	return keystate;
}

uint32_t KeyPadController::getKeyPressed(uint16_t keystate, uint8_t *cptr)
{
	uint8_t ci = 0;
	uint32_t count = 0;
	uint8_t keyPadState = 0;

	for (uint8_t i = 0; i < 16; i++)
	{
		// If key reading is 0 (pressed)
		if (0x1 == (keystate & 0x1))
		{
			count++; // Count the number of pressed keys
			ci = i;
		}
		// Increment through keystate bits
		keystate >>= 1;
	}

	if (count > 1)
	{
		// Multiple keys pressed, cannot differentiate which
		keyPadState = static_cast<uint8_t>(KEY_PAD_STATUS::MULTI_KEY);
	}
	else if (count == 0)
	{
		// No key pressed
		keyPadState = static_cast<uint8_t>(KEY_PAD_STATUS::NO_KEY);
	}
	else
	{
	  // One key pressed
	  if (kypdInterface.keytable_loaded == TRUE)
	  {
		 *cptr = kypdInterface.keytable[ci]; // Return human-readable key label
	  }
	  else
	  {
		 *cptr = ci; // Return index of pressed key
	  }
		keyPadState = static_cast<uint8_t>(KEY_PAD_STATUS::SINGLE_KEY);
	}
	return keyPadState;
}

bool KeyPadController::selfTest()
{
	uint16_t keystate;
	int16_t status;
	int16_t last_status = static_cast<int16_t>(KEY_PAD_STATUS::NO_KEY);
	uint8_t key, last_key = 'x';
	bool result = true;

	Xil_Out32(kypdInterface.GPIO_addr, 0xF);

	xil_printf("Pmod KYPD demo started. Press any key on the Keypad.\r\n");

	keystate = getKeyStates();
	status = getKeyPressed(keystate, &key);

	// Print key detect if a new key is pressed or if status has changed
	if (status == static_cast<uint8_t>(KEY_PAD_STATUS::SINGLE_KEY)	&& (status != last_status || key != last_key))
	{
		xil_printf("Key Pressed: %c\r\n", (char) key);
		last_key = key;
	}
	else if (status == static_cast<uint8_t>(KEY_PAD_STATUS::MULTI_KEY) && status != last_status)
		xil_printf("Error: Multiple keys pressed\r\n");

	last_status = status;
	return result;
}
}

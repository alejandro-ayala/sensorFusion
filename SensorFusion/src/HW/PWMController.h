#include "TestInclude.h"

namespace Hardware
{

class PWMController
{
private:

public:
	PWMController(){};
	TVIRTUAL ~PWMController(){};

	TVIRTUAL void setDutyCicle(uint32_t dutyCycle);
	TVIRTUAL void setFrequency(uint32_t frequency);
};

}

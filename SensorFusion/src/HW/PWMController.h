#include "TestInclude.h"
#include "IController.h"

namespace Hardware
{

class PWMController : public IController
{
private:

public:
	PWMController();
	TVIRTUAL ~PWMController();

	TVIRTUAL void initialize() override;
	TVIRTUAL void setDutyCicle(uint32_t dutyCycle);
	TVIRTUAL void setFrequency(uint32_t frequency);
};

}

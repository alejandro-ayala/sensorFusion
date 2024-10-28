#include <hardware_abstraction/Controllers/IController.h>
#include "TestInclude.h"

namespace hardware_abstraction
{
namespace Controllers
{

class PWMController : public IController
{
private:
	uint32_t baseAddr;
public:
	PWMController();
	TVIRTUAL ~PWMController();

	TVIRTUAL void initialize() override;
	TVIRTUAL void enable();
	TVIRTUAL void disable();
	TVIRTUAL void setDutyCicle(uint32_t clocks, uint32_t pwmIndex);
	TVIRTUAL void setPeriod(uint32_t clocks);
	TVIRTUAL uint32_t getDutyCicle(uint32_t pwmIndex);
	TVIRTUAL uint32_t getPeriod();
	bool selfTest() override;
};
}
}

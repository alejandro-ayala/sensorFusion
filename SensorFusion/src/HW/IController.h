
#pragma once
#include "Definitions.h"
namespace Hardware
{
class IController {
private:
	cstring name;
public:
	IController(cstring name) : name(name){};
	virtual ~IController(){};

	virtual void initialize() = 0;
};
}

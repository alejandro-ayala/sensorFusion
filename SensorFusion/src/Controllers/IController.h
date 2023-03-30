
#pragma once
#include "Definitions.h"
namespace Controllers
{
class IController {
public:
	cstring name;
	bool initialized;

	IController(cstring name) : name(name), initialized(false){};
	virtual ~IController(){};

	virtual void initialize() = 0;
	virtual bool selfTest()   = 0;
};
}

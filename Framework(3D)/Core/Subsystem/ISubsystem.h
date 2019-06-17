#pragma once
#include "Framework.h"

class ISubsystem
{
public:
	ISubsystem(class Context *context) : context(context) {

	}
	virtual ~ISubsystem() = default;

	virtual auto Initialize() -> const bool = 0;


protected:
	class Context *context;
};
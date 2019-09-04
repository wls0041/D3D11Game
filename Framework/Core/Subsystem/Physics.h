#pragma once
#include "ISubsystem.h"

class Physics final : public ISubsystem
{
public:
	Physics(class Context *context);
	~Physics();

	const bool Initialize() override;
};
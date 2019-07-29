#pragma once
#include "ISubsystem.h"

class Renderer final
{
public:
	Renderer(class Context *context);
	~Renderer();

	void Initialize() override;

private:

};
#include "Framework.h"
#include "Engine.h"

Engine::Engine()
{
	context = new Context();
	context->RegisterSubsystem<Graphics>();
	context->RegisterSubsystem<Input>();

	context->InitializeSubsystems();
}

Engine::~Engine()
{
	SAFE_DELETE(context);
}

void Engine::Update()
{
}

void Engine::Render()
{
}

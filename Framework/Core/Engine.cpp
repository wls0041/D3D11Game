#include "Framework.h"
#include "Engine.h"
#include "../Scene/Scene.h"

Engine::Engine()
{
	context = new Context();

	context->RegisterSubsystem<Timer>();
	context->RegisterSubsystem<Input>();
	context->RegisterSubsystem<Thread>();
    context->RegisterSubsystem<Graphics>();
	context->RegisterSubsystem<ResourceManager>();
	context->RegisterSubsystem<SceneManager>();
	
	context->InitializeSubsystems();
}
 
Engine::~Engine()
{
    SAFE_DELETE(context);
}

void Engine::Update()
{
	FIRE_EVENT(EventType::Frame_Start);
	FIRE_EVENT(EventType::Update);
	FIRE_EVENT(EventType::Render);
	FIRE_EVENT(EventType::Frame_End);
}

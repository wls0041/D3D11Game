#include "Framework.h"
#include "Engine.h"
#include "../Scene/Scene.h"

uint  Engine::engine_flags = 0U;

Engine::Engine()
{
	engine_flags =
		EngineFlags_Update |
		EngineFlags_Render |
		EngineFlags_Physics;

	context = new Context();

	context->RegisterSubsystem<Timer>();
	context->RegisterSubsystem<Input>();
	context->RegisterSubsystem<Thread>();
    context->RegisterSubsystem<Graphics>();
	context->RegisterSubsystem<Scripting>();
	context->RegisterSubsystem<ResourceManager>();
	context->RegisterSubsystem<SceneManager>();
	context->RegisterSubsystem<Renderer>();
	context->RegisterSubsystem<Physics>();
	
	context->InitializeSubsystems();
}
 
Engine::~Engine()
{
    SAFE_DELETE(context);
}

void Engine::Update()
{
	FIRE_EVENT(EventType::Frame_Start);
	if (IsFlagsEnabled(EngineFlags_Update)) FIRE_EVENT(EventType::Update);
	if (IsFlagsEnabled(EngineFlags_Render)) FIRE_EVENT(EventType::Render);
	FIRE_EVENT(EventType::Frame_End);
}

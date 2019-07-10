#include "Framework.h"
#include "Engine.h"
#include "../Scene/Scene.h"

Engine::Engine()
{
    context = new Context();
    context->RegisterSubsystem<Graphics>();
    input = context->RegisterSubsystem<Input>();
	timer = context->RegisterSubsystem<Timer>();
    context->InitializeSubsystems();

    scene = new Scene(context);
}

Engine::~Engine()
{
    SAFE_DELETE(scene);
    SAFE_DELETE(context);
}

void Engine::Update()
{
	timer->Update();
    input->Update();
    scene->Update();
}

void Engine::Render()
{
    scene->Render();
}

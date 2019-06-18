#include "stdafx.h"
#include "Editor.h"

Editor::Editor()
{
	engine = new Engine();
	context = engine->GetContext();
	graphics = context->GetSubsystem<Graphics>();


}

Editor::~Editor()
{
}

void Editor::Update()
{
	engine->Update();
}

void Editor::Render()
{
	graphics->Begin();
	{
		engine->Render();
	}
	graphics->End();
}

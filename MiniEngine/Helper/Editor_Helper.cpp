#include "stdafx.h"
#include "Editor_Helper.h"
#include "Scene/Actor.h"

void Editor_Helper::Initialize(Context * context)
{
	this->context = context;
	this->resource_manager = context->GetSubsystem<ResourceManager>();
	this->scene_manager = context->GetSubsystem<SceneManager>();
	this->thread = context->GetSubsystem<Thread>();
	this->renderer = context->GetSubsystem<Renderer>();
}

void Editor_Helper::LoadModel(const std::string & path)
{
	thread->AddTask([this, path]() {
		resource_manager->Load<Model>(path);
	});
}

#include "Framework.h"
#include "Renderable.h"

Renderable::Renderable(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
{
}

Renderable::~Renderable()
{
}

void Renderable::OnStart()
{
}

void Renderable::OnUpdate()
{
}

void Renderable::OnStop()
{
}

void Renderable::SetMaterial(const std::string & path)
{
	material = context->GetSubsystem<ResourceManager>()->Load<Material>(path);
}

void Renderable::SetStandardMaterial()
{
	if (auto mat = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Material>("Standard"))
	{
		material = mat;
		return;
	}

	material = std::make_shared<Material>(context);
	material->SetAlbedoColor({ 0.6f, 0.6f, 0.6f, 1.0f });
	material->SetStandardShader();
	material->SetResourceName("Standard");

	context->GetSubsystem<ResourceManager>()->RegisterResource(material);
}

void Renderable::SetMesh(const std::string & path)
{
	mesh = context->GetSubsystem<ResourceManager>()->Load<Mesh>(path);
}

void Renderable::SetStandardMesh(const MeshType & type)
{
	mesh = std::make_shared<Mesh>(context);
	mesh->SetMeshType(type);
	mesh->CreateBuffers();

	context->GetSubsystem<ResourceManager>()->RegisterResource(mesh);
}

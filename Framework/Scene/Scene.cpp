#include "Framework.h"
#include "Scene.h"
#include "Actor.h"
#include "./Component/Camera.h"
#include "./Component/Transform.h"
#include "./Component/Light.h"

Scene::Scene(Context * context)
	: context(context)
	, name(NOT_ASSIGNED_STR)
	, is_editor_mode(true)
	, is_added(false)
{
	renderer = context->GetSubsystem<Renderer>();
	CreateCamera();
	CreateDirectionalLight();
}

Scene::~Scene()
{
	actors.clear();
	actors.shrink_to_fit();
}

void Scene::SaveToFile(const std::string & path)
{
}

void Scene::LoadFromFile(const std::string & path)
{
}

auto Scene::GetRoots() const -> const std::vector<std::shared_ptr<class Actor>>
{
	std::vector<std::shared_ptr<Actor>> root_actors; //root만 따로 처리하는 이유 => hierarchy에서 root는 따로 모아 처리하기 때문
	for (const auto& actor : actors)
		if (actor->GetTransform()->IsRoot())
			root_actors.emplace_back(actor);
	
	return root_actors;
}

auto Scene::GetActorFromID(const uint & id) const -> const std::shared_ptr<class Actor>
{
	for (const auto& actor : actors)
		if (actor->GetID() == id) return actor;

	return nullptr;
}

auto Scene::CreateActor() -> std::shared_ptr<class Actor>
{
	auto actor = std::make_shared<Actor>(context);
	actor->Initialize(actor->AddComponent<Transform>());
	AddActor(actor);

	return actor;
}

void Scene::AddActor(const std::shared_ptr<class Actor>& actor)
{
	actors.emplace_back(actor);
	is_added = true;
}

void Scene::RemoveActor(const std::shared_ptr<class Actor>& actor)
{
}

void Scene::Update()
{
	bool is_started = Engine::IsFlagsEnabled(EngineFlags_Game) && is_editor_mode;
	bool is_stopped = !Engine::IsFlagsEnabled(EngineFlags_Game) && !is_editor_mode;
	is_editor_mode = !Engine::IsFlagsEnabled(EngineFlags_Game);

	if (is_added) { renderer->AcquireRenderables(this); is_added = false; }
	if (is_started) for (auto& actor : actors) actor->Start();  
	if (is_stopped) for (auto& actor : actors) actor->Stop(); 

	for (auto& actor : actors) actor->Update();

}

auto Scene::CreateCamera() -> std::shared_ptr<class Actor>
{
	auto camera = CreateActor();
	camera->SetName("MainCamera");
	camera->GetTransform()->SetTranslation(Vector3(0.0f, 0.0f, -2.0));
	camera->AddComponent<Camera>();

	return camera;
}

auto Scene::CreateDirectionalLight() -> std::shared_ptr<class Actor>
{
	auto light = CreateActor();
	light->SetName("Directional Light");
	light->GetTransform()->SetRotation(Quaternion::QuaternionFromEulerAngle(Vector3(30.0f, 0.0f, 0.0f)));
	light->GetTransform()->SetTranslation(Vector3(0.0f, 10.0f, -5.0f));

	auto component = light->AddComponent<Light>();
	component->SetLightType(LightType::Directional);
	component->SetIntensity(1.5f);

	return light;
}

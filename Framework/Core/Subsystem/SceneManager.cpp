#include "Framework.h"
#include "SceneManager.h"
#include "../../Scene/Scene.h"

SceneManager::SceneManager(Context * context)
	: ISubsystem(context)
{
	SUBSCRIBE_TO_EVENT(EventType::Update, EVENT_HANDLER(Update));
}

SceneManager::~SceneManager()
{
}

const bool SceneManager::Initialize()
{
	RegisterScene("First");
	SetCurrentScene("First");

	return true;
}

auto SceneManager::GetCurrentScene() const -> class Scene *
{
	return current_scene.expired() ? nullptr : current_scene.lock().get(); //lock -> shared ptr로 바꿔주는 놈
}

void SceneManager::SetCurrentScene(const std::string & scene_name)
{
	if (scenes.find(scene_name) == scenes.end()) {
		LOG_ERROR("Failed To Find Scene");
		return;
	}
	current_scene = scenes[scene_name];
}

auto SceneManager::RegisterScene(const std::string & scene_name) -> class Scene *
{
	//insert 반환값 = std::pair<pair_ib, bool>  ... iterator(ib)
	auto pair_ib = scenes.insert(std::make_pair(scene_name, std::make_shared<Scene>(context)));
	if (!pair_ib.second) LOG_ERROR("Already Have Scene"); //실패해도 해당 위치의 이터를 반환
	
	return pair_ib.first->second.get();
}

void SceneManager::Update()
{
	if (!current_scene.expired()) current_scene.lock()->Update();	
}

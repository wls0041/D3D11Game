#include "stdafx.h"
#include "Widget_Hierarchy.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

namespace Hierarchy { //객체지향적으로 좋지는 않으나 귀찮아서 묶어둠. 
	static Scene *current_scene = nullptr;
	static Actor *copied_actor	= nullptr;
	static Actor *hovered_actor = nullptr; //마우스가 올라가 있는 액터
	static Actor *clicked_actor = nullptr;
	static bool rename_popup	= false;
}

Widget_Hierarchy::Widget_Hierarchy(Context * context)
    : IWidget(context)
{
    title = "Hierarchy";
}

void Widget_Hierarchy::Render()
{
	if (!is_visible) return;

	ShowHierarchy();
	if (ImGui::IsMouseReleased(0) && Hierarchy::clicked_actor) {
		if (Hierarchy::hovered_actor && Hierarchy::hovered_actor->GetID() == Hierarchy::clicked_actor->GetID())
			SelectedItem(Hierarchy::clicked_actor->shared_from_this());

		Hierarchy::clicked_actor = nullptr;
	}
}

void Widget_Hierarchy::ShowHierarchy()
{
	Hierarchy::current_scene = Editor_Helper::Get().scene_manager->GetCurrentScene();

	if (ImGui::CollapsingHeader(Hierarchy::current_scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		auto roots = Hierarchy::current_scene->GetRoots();
		for (const auto &root : roots) AddActor(root);
	}

	ClickedItem();
	ShowMenuPopup();
}

void Widget_Hierarchy::ShowMenuPopup()
{
	if (!ImGui::BeginPopup("Hierarchy Menu Popup")) return;

	auto selected_actor = Editor_Helper::Get().select_actor.lock();
	const auto on_actor = selected_actor != nullptr;

	if (ImGui::MenuItem("Copy", nullptr, nullptr, on_actor)) { //on_actor -> 팝업에서 비활성화, 활성화 체크

	}

	if (ImGui::MenuItem("Paste", nullptr, nullptr, on_actor)) {}
	if (ImGui::MenuItem("Delete", nullptr, nullptr, on_actor)) {}
	if (ImGui::MenuItem("Rename", nullptr, nullptr, on_actor)) {}

	ImGui::Separator();

	if (ImGui::BeginMenu("Create 3D")) {
		if (ImGui::MenuItem("Empty"))		CreateEmpty();
		if (ImGui::MenuItem("Cube"))		CreateStandardMesh(MeshType::Cube);
		if (ImGui::MenuItem("Cylinder"))	CreateStandardMesh(MeshType::Cylinder);
		if (ImGui::MenuItem("Cone"))		CreateStandardMesh(MeshType::Cone);
		if (ImGui::MenuItem("Sphere"))		CreateStandardMesh(MeshType::Sphere);
		if (ImGui::MenuItem("Plane"))		CreateStandardMesh(MeshType::Plane);
		if (ImGui::MenuItem("Capsule"))		CreateStandardMesh(MeshType::Capsule);

		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Camera"))			CreateCamera();

	ImGui::EndPopup();
}

void Widget_Hierarchy::AddActor(const std::shared_ptr<class Actor>& actor)
{
	if (!actor) return;

	auto childs = actor->GetTransform()->GetChilds();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	flags = childs.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow; //자식이 있으면 옆에 삼각형 만들고 누르면 열리게 함

	if (const auto selected_actor = Editor_Helper::Get().select_actor.lock()) {
		//TODO : transform 만들고 //클릭관련 세팅
	}

	const auto is_node_open = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(actor->GetID())), flags, actor->GetName().c_str());

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) Hierarchy::hovered_actor = actor.get();
	if (is_node_open) {
		for (const auto &child : childs) AddActor(child->GetActor()->shared_from_this());
		ImGui::TreePop();
	}
}

void Widget_Hierarchy::ClickedItem()
{
	const auto is_window_hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenBlockedByPopup); //input은 framework에서만 쓰고 engine은 imgui에 맞춰서 쓸 예정
	const auto is_left_click = ImGui::IsMouseClicked(0);
	const auto is_right_click = ImGui::IsMouseClicked(1);

	if (!is_window_hovered) return;
	if (is_left_click && Hierarchy::hovered_actor) Hierarchy::clicked_actor = Hierarchy::hovered_actor;
	if (is_right_click) {
		if (Hierarchy::hovered_actor) SelectedItem(Hierarchy::hovered_actor->shared_from_this());
		ImGui::OpenPopup("Hierarchy Menu Popup");
		
	}

	if ((is_left_click || is_right_click) && !Hierarchy::hovered_actor) SelectedItem(empty_actor); //딴데 클릭하면 셀렉이 꺼지게
}

void Widget_Hierarchy::SelectedItem(const std::shared_ptr<class Actor> &actor)
{
	is_expand_to_show_actor = true;
	Editor_Helper::Get().select_actor = actor;
}

auto Widget_Hierarchy::CreateEmpty() -> std::shared_ptr<class Actor>
{
	if (auto scene = Hierarchy::current_scene) {
		auto actor = scene->CreateActor();
		actor->SetName("Empty");

		return actor;
	}

	return nullptr;
}

void Widget_Hierarchy::CreateStandardMesh(const MeshType & type)
{
	const auto directory = Editor_Helper::Get().resource_manager->GetAssetDirectory(AssetType::Model);

	switch (type)
	{
	case MeshType::Cube:		Editor_Helper::Get().LoadModel(directory + "Cube.fbx"); break;
	case MeshType::Cylinder:	Editor_Helper::Get().LoadModel(directory + "Cylinder.fbx"); break;
	case MeshType::Cone:		Editor_Helper::Get().LoadModel(directory + "Cone.fbx"); break;
	case MeshType::Sphere:		Editor_Helper::Get().LoadModel(directory + "Sphere.fbx"); break;
	case MeshType::Plane:		Editor_Helper::Get().LoadModel(directory + "Plane.fbx"); break;
	case MeshType::Capsule:		Editor_Helper::Get().LoadModel(directory + "Capsule.fbx"); break;
	}
}

void Widget_Hierarchy::CreateCamera()
{

}

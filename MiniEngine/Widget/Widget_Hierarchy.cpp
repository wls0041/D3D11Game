#include "stdafx.h"
#include "Widget_Hierarchy.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

Widget_Hierarchy::Widget_Hierarchy(Context * context)
    : IWidget(context)
{
    title = "Hierarchy";
}

void Widget_Hierarchy::Render()
{
	if (!is_visible) return;

	ShowHierarchy();
}

void Widget_Hierarchy::ShowHierarchy()
{

}

void Widget_Hierarchy::ShowPopup()
{
}

void Widget_Hierarchy::AddActor(const std::shared_ptr<class Actor>& actor)
{
	if (!actor) return;

	auto childs = actor->GetTransform()->GetChilds();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_AllowItemOverlap;
	flags = childs.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow; //자식이 있으면 옆에 삼각형 만들고 누르면 열리게 함

	if (const auto selected_actor = Editor_Helper::Get().select_actor) {
		//TODO : transform 만들고
	}

	const auto is_node_open = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(actor->GetID())), flags, actor->GetName().c_str());

	if (is_node_open) {
		for (const auto &child : childs) AddActor(child->GetActor()->shared_from_this());
		ImGui::TreePop();
	}
}

auto Widget_Hierarchy::CreateEmpty() -> std::shared_ptr<class Actor>
{
	return std::shared_ptr<class Actor>();
}

void Widget_Hierarchy::CreateStandardMesh(const MeshType & type)
{
}

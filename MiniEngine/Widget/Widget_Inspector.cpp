#include "stdafx.h"
#include "Widget_Inspector.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

namespace Inspector {
	static std::string menu_id;
	static float column = 140.0f;
	static float max_width = 100.0f;
	static std::shared_ptr<IComponent> copied_component;

	inline void ComponentMenuOption(const std::string &id, const std::shared_ptr<IComponent> &component, const bool &removable) {
		if (ImGui::BeginPopup(id.c_str())) {
			if (removable) {
				if (ImGui::MenuItem("Remove")) {
					//TODO :
				}
				if (ImGui::MenuItem("Copy Attribute")) copied_component = component;

				if (ImGui::MenuItem("Paste Attribute")) {
					if (copied_component && copied_component->GetComponentType() == component->GetComponentType())
						component->SetAttributes(copied_component->GetAttributes());
				}
				
				ImGui::EndPopup();
			}
		}
	}

	inline const bool ComponentBegin(const std::string &name, const IconType &type, const std::shared_ptr<IComponent> &component, const bool &option = true, const bool &removable = true) {
		const auto collapsed_header = ImGui::CollapsingHeader(name.c_str(), ImGuiButtonFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);
		
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		const auto original_pen_y = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(original_pen_y + 5.0f);

		//TODO : IconButton

		if (option) {
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.973f);
			ImGui::SetCursorPosY(original_pen_y + 1.5f);

			//TOdO : IconButton

			if (menu_id == name) ComponentMenuOption(menu_id, component, removable);
		}

		return collapsed_header;
	}

	inline void ComponentEnd() {
		ImGui::Separator();
	}
}

Widget_Inspector::Widget_Inspector(Context * context)
    : IWidget(context)
{
    title = "Inspector";
}

void Widget_Inspector::Render()
{
	ImGui::PushItemWidth(Inspector::max_width);

	if (auto actor = Editor_Helper::Get().select_actor.lock()) {
		ShowTransform(actor->GetComponent<Transform>());
	}
	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (Inspector::ComponentBegin("Transform",  IconType::Component_Transform, transform, true, false)) {

	}
	Inspector::ComponentEnd();
}

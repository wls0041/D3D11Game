#include "stdafx.h"
#include "Widget_Inspector.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Script.h"

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

		Icon_Provider::Get().Image(type, 15.0f);

		if (option) {
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.973f);
			ImGui::SetCursorPosY(original_pen_y + 1.5f);

			if (Icon_Provider::Get().ImageButton(name.c_str(), IconType::Component_Option, 12.0f)) {
				Inspector::menu_id = name;
				ImGui::OpenPopup(Inspector::menu_id.c_str());
			}

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

	if (auto actor = Editor_Helper::Get().select_actor.lock())
	{
		ShowTransform(actor->GetComponent<Transform>());
		ShowScript(actor->GetComponent<Script>());
	}

	ShowAddComponentButton();
	AddComponentDragDrop();

	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (!transform) return;

	if (Inspector::ComponentBegin("Transform",  IconType::Component_Transform, transform, true, false)) {
		auto position = transform->GetTranslation();
		auto rotation = transform->GetRotation().ToEulerAngle();
		auto scale = transform->GetScale();
		
		const auto show_float = [](const char *id, const char *label, float *value) { //id. 같은 xyz사용 하므로 헷갈리지 않도록 id사용
			ImGui::PushItemWidth(100.0f);
			ImGui::PushID(id);
			ImGui::InputFloat(label, value, 1.0f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		ImGui::TextUnformatted("Position");
		ImGui::SameLine(70.0f); show_float("##pos_x", "X", &position.x);
		ImGui::SameLine();		show_float("##pos_y", "Y", &position.y);
		ImGui::SameLine();		show_float("##pos_z", "Z", &position.z);

		ImGui::TextUnformatted("Rotation");
		ImGui::SameLine(70.0f); show_float("##rot_x", "X", &rotation.x);
		ImGui::SameLine();		show_float("##rot_y", "Y", &rotation.y);
		ImGui::SameLine();		show_float("##rot_z", "Z", &rotation.z);

		ImGui::TextUnformatted("Scale");
		ImGui::SameLine(70.0f); show_float("##sck_x", "X", &scale.x);
		ImGui::SameLine();		show_float("##sck_y", "Y", &scale.y);
		ImGui::SameLine();		show_float("##sck_z", "Z", &scale.z);
	
		transform->SetTranslation(position);
		transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation));
		transform->SetScale(scale);
	}

	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowScript(std::shared_ptr<class Script>& script)
{
	if (!script) return;
	if (Inspector::ComponentBegin(script->GetScriptName(), IconType::Component_Script, script)) {
		auto script_name = script->GetScriptName();

		ImGui::TextUnformatted("Script");
		ImGui::SameLine();
		ImGui::PushID("##ScriptName");
		ImGui::PushItemWidth(200.0f);
		{
			ImGui::InputText("", &script_name, ImGuiInputTextFlags_ReadOnly);

			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script))
				script->SetScript(std::get<const char*>(payload->data)); //dragdrop은 바로 위의 특정 아이템에 이어지므로 inputText바로 아래에 적어야 한다.

			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{

			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
	}
	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowAddComponentButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

	if (ImGui::Button("Add Component")) ImGui::OpenPopup("##ComponentPopup");

	ShowComponentPopup();
}

void Widget_Inspector::ShowComponentPopup()
{
	if (ImGui::BeginPopup("##ComponentPopup")) {
		if (auto actor = Editor_Helper::Get().select_actor.lock()) {
			if (ImGui::MenuItem("Script")) actor->AddComponent<Script>();
		}

		ImGui::EndPopup();
	}
}

void Widget_Inspector::AddComponentDragDrop()
{
	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script)) {
		if (auto script_component = Editor_Helper::Get().select_actor.lock()->AddComponent<Script>())
			script_component->SetScript(std::get<const char*>(payload->data));
	}
}

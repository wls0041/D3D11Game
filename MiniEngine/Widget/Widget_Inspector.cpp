#include "stdafx.h"
#include "Widget_Inspector.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/RigidBody.h"
#include "Scene/Component/Collider.h"
#include "Scene/Component/Script.h"

namespace Inspector
{
	static std::string menu_id;
	static float column = 140.0f;
	static float max_width = 100.0f;
	static std::shared_ptr<IComponent> copied_component;

	inline void ComponentMenuOption(const std::string& id, const std::shared_ptr<IComponent>& component, const bool& removable)
	{
		if (ImGui::BeginPopup(id.c_str()))
		{
			if (removable)
			{
				if (ImGui::MenuItem("Remove"))
				{
					//TODO :
				}
			}

			if (ImGui::MenuItem("Copy Attribute"))
				copied_component = component;

			if (ImGui::MenuItem("Paste Attribute"))
			{
				if (copied_component && copied_component->GetComponentType() == component->GetComponentType())
					component->SetAttributes(copied_component->GetAttributes());
			}

			ImGui::EndPopup();
		}
	}

	inline const bool ComponentBegin(const std::string& name, const IconType& type, const std::shared_ptr<IComponent>& component, const bool& option = true, const bool& removable = true)
	{
		const auto collapsed_header = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		const auto original_pen_y = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(original_pen_y + 5.0f);

		Icon_Provider::Get().Image(type, 15.0f);

		if (option)
		{
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() * 0.973f);
			ImGui::SetCursorPosY(original_pen_y + 1.5f);
			if (Icon_Provider::Get().ImageButton(name.c_str(), IconType::Component_Option, 12.0f))
			{
				Inspector::menu_id = name;
				ImGui::OpenPopup(Inspector::menu_id.c_str());
			}

			if (menu_id == name)
				ComponentMenuOption(menu_id, component, removable);
		}

		return collapsed_header;
	}

	inline void ComponentEnd()
	{
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
		ShowRenderable(actor->GetComponent<Renderable>());
		ShowRigidBody(actor->GetComponent<RigidBody>());
		ShowCollider(actor->GetComponent<Collider>());
	}

	ShowAddComponentButton();
	AddComponentDragDrop();

	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (!transform)
		return;

	if (Inspector::ComponentBegin("Transform", IconType::Component_Transform, transform, true, false))
	{
		auto position = transform->GetTranslation();
		auto rotation = transform->GetRotation().ToEulerAngle();
		auto scale = transform->GetScale();

		const auto show_float = [](const char* id, const char* label, float* value)
		{
			ImGui::PushItemWidth(100.0f);
			ImGui::PushID(id);
			ImGui::InputFloat(label, value, 1.0f, 1.0f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		ImGui::TextUnformatted("Position");
		ImGui::SameLine(70.0f); show_float("##pos_x", "X", &position.x);
		ImGui::SameLine();      show_float("##pos_y", "Y", &position.y);
		ImGui::SameLine();      show_float("##pos_z", "Z", &position.z);

		ImGui::TextUnformatted("Rotation");
		ImGui::SameLine(70.0f); show_float("##rot_x", "X", &rotation.x);
		ImGui::SameLine();      show_float("##rot_y", "Y", &rotation.y);
		ImGui::SameLine();      show_float("##rot_z", "Z", &rotation.z);

		ImGui::TextUnformatted("Scale");
		ImGui::SameLine(70.0f); show_float("##scl_x", "X", &scale.x);
		ImGui::SameLine();      show_float("##scl_y", "Y", &scale.y);
		ImGui::SameLine();      show_float("##scl_z", "Z", &scale.z);

		transform->SetTranslation(position);
		transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation));
		transform->SetScale(scale);
	}
	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowScript(std::shared_ptr<class Script>& script)
{
	if (!script)
		return;

	if (Inspector::ComponentBegin(script->GetScriptName(), IconType::Component_Script, script))
	{
		auto script_name = script->GetScriptName();

		ImGui::TextUnformatted("Script");
		ImGui::SameLine();
		ImGui::PushID("##ScriptName");
		ImGui::PushItemWidth(200.0f);
		{
			ImGui::InputText("", &script_name, ImGuiInputTextFlags_ReadOnly);

			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script))
				script->SetScript(std::get<const char*>(payload->data));//dragdrop은 바로 위의 특정 아이템에 이어지므로 inputText바로 아래에 적어야 한다.

			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{
				//TODO :
			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
	}
	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowRenderable(std::shared_ptr<class Renderable>& renderable)
{
}

void Widget_Inspector::ShowMaterial(std::shared_ptr<class Material>& material)
{
}

void Widget_Inspector::ShowRigidBody(std::shared_ptr<class RigidBody>& rigid_body)
{
	if (!rigid_body)
		return;

	if (Inspector::ComponentBegin("RigidBody", IconType::Component_RigidBody, rigid_body))
	{
		auto mass = rigid_body->GetMass();
		auto friction = rigid_body->GetFriction();
		auto friction_rolling = rigid_body->GetFrictionRolling();
		auto restitution = rigid_body->GetRestitution();
		auto is_use_gravity = rigid_body->IsUseGravity();
		auto is_kinematic = rigid_body->IsKinematic();

		//Mass
		ImGui::TextUnformatted("Mass");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Mass", &mass, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//Friction
		ImGui::TextUnformatted("Friction");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Friction", &friction, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//Friction Rolling
		ImGui::TextUnformatted("Friction Rolling");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Friction Rolling", &friction_rolling, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//Restitution
		ImGui::TextUnformatted("Restitution");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Restitution", &restitution, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//Use Gravity
		ImGui::TextUnformatted("Use Gravity");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##Use Gravity", &is_use_gravity);

		//Is Kinematic
		ImGui::TextUnformatted("Is Kinematic");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##Is Kinematic", &is_kinematic);

		if (mass != rigid_body->GetMass())                                      rigid_body->SetMass(mass);
		if (friction != rigid_body->GetFriction())                              rigid_body->SetFriction(friction);
		if (friction_rolling != rigid_body->GetFrictionRolling())               rigid_body->SetFrictionRolling(friction_rolling);
		if (restitution != rigid_body->GetRestitution())                        rigid_body->SetRestitution(restitution);
		if (is_use_gravity != rigid_body->IsUseGravity())                       rigid_body->SetUseGravity(is_use_gravity);
		if (is_kinematic != rigid_body->IsKinematic())                          rigid_body->SetKinematic(is_kinematic);
	}
	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowCollider(std::shared_ptr<class Collider>& collider)
{
	if (!collider)
		return;

	if (Inspector::ComponentBegin("Collider", IconType::Component_Collider, collider))
	{
		static std::vector<const char*> shape_types
		{
			"Box",
			"Sphere",
			"Static Plane",
			"Cylinder",
			"Capsule",
			"Cone",
			"Mesh"
		};

		auto shape_type = shape_types[static_cast<uint>(collider->GetShapeType())];
		auto optimize = collider->IsOptimize();
		auto collider_center = collider->GetCenter();
		auto collider_size = collider->GetSize();

		//Shape Type
		ImGui::TextUnformatted("Shape Type");
		ImGui::PushItemWidth(110.0f);
		ImGui::SameLine(140.0f);
		if (ImGui::BeginCombo("##Shape Type", shape_type))
		{
			for (uint i = 0; i < static_cast<uint>(shape_types.size()); i++)
			{
				const auto bSelected = shape_type == shape_types[i];
				if (ImGui::Selectable(shape_types[i], bSelected))
				{
					shape_type = shape_types[i];
					collider->SetShapeType(static_cast<ColliderShapeType>(i));
				}

				if (bSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		//Collider Center
		ImGui::TextUnformatted("Center");
		ImGui::SameLine(140.0f);
		ImGui::PushID("CenterX"); ImGui::InputFloat("X", &collider_center.x, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();
		ImGui::SameLine(140.0f);
		ImGui::PushID("CenterY"); ImGui::InputFloat("Y", &collider_center.y, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();
		ImGui::SameLine(140.0f);
		ImGui::PushID("CenterZ"); ImGui::InputFloat("Z", &collider_center.z, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();

		//Collider Size
		ImGui::TextUnformatted("Size");
		ImGui::SameLine(140.0f);
		ImGui::PushID("SizeX"); ImGui::InputFloat("X", &collider_size.x, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();
		ImGui::SameLine(140.0f);
		ImGui::PushID("SizeY"); ImGui::InputFloat("Y", &collider_size.y, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();
		ImGui::SameLine(140.0f);
		ImGui::PushID("SizeZ"); ImGui::InputFloat("Z", &collider_size.z, 0.1f, 0.1f, "%.3f", ImGuiInputTextFlags_CharsDecimal); ImGui::PopID();

		//Optimize
		if (collider->GetShapeType() == ColliderShapeType::Mesh)
		{
			ImGui::TextUnformatted("Optimize");
			ImGui::SameLine(140.0f);
			ImGui::Checkbox("##Optimize", &optimize);
		}

		if (collider_center != collider->GetCenter())   collider->SetCenter(collider_center);
		if (collider_size != collider->GetSize())       collider->SetSize(collider_size);
		if (optimize != collider->IsOptimize())         collider->SetIsOptimize(optimize);
	}
	Inspector::ComponentEnd();
}

void Widget_Inspector::ShowAddComponentButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("##ComponentPopup");

	ShowComponentPopup();
}

void Widget_Inspector::ShowComponentPopup()
{
	if (ImGui::BeginPopup("##ComponentPopup"))
	{
		if (auto actor = Editor_Helper::Get().select_actor.lock())
		{
			if (ImGui::MenuItem("Script"))
				actor->AddComponent<Script>();

			if (ImGui::BeginMenu("Physics"))
			{
				if (ImGui::MenuItem("RigidBody"))
					actor->AddComponent<RigidBody>();
				if (ImGui::MenuItem("Collider"))
					actor->AddComponent<Collider>();

				ImGui::EndMenu();
			}
		}
		ImGui::EndPopup();
	}
}

void Widget_Inspector::AddComponentDragDrop()
{
	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script))
	{
		if (auto script_component = Editor_Helper::Get().select_actor.lock()->AddComponent<Script>())
			script_component->SetScript(std::get<const char*>(payload->data));
	}
}

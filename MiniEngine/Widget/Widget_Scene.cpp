#include "stdafx.h"
#include "Widget_Scene.h"
#include "Imgui/ImGuizmo.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Transform.h"

Widget_Scene::Widget_Scene(Context * context)
    : IWidget(context)
{
    title = "Scene";
	window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse; //마우스 움직일 때 자동으로 스크롤 되는 것 방지

	timer = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();

	///////////////////////////////Test//////////////////////////////////
}

void Widget_Scene::Render()
{
	ShowFrame();
	ShowTransformGizmo();
}

void Widget_Scene::ShowFrame()
{
	auto frame_position_x = static_cast<uint>(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x); //창 기준 마우스 위치이므로 window 포지션을 더해야 현재 위치 나옴
	auto frame_position_y = static_cast<uint>(ImGui::GetCursorPos().y + ImGui::GetWindowPos().y); 

	auto frame_width = static_cast<uint>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto frame_height = static_cast<uint>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	//더 정밀하게 하려면 한계를 잡아줘야함.(0이면 보이지 않고 일정이상 넘어가면 overflow)
	frame_width -= frame_width % 2 != 0 ? 1 : 0; //기본적으로 이미지 사이즈가 2의 배수이므로 이에 맞춰주기 위한 처리 
	frame_height -= frame_height % 2 != 0 ? 1 : 0;

	renderer->SetEditorOffset(Vector2(static_cast<float>(frame_position_x), static_cast<float>(frame_position_y))); //추후 피킹에 사용

	if (frame_counter >= 0.1f) //창의 크기가 바뀌면 내부 크기도 바꾸는데 이에 최소시간을 줘서 낭비를 막음
	{
		renderer->SetResolution(frame_width, frame_height);
		frame_counter = 0.0f;
	}
	frame_counter += timer->GetDeltaTimeSec();

	ImGui::Image( //출력
		renderer->GetFrameResource(),
		ImVec2(static_cast<float>(frame_width), static_cast<float>(frame_height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(1, 1, 1, 1),
		ImColor(255, 0, 0, 255)
	);

	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
		Editor_Helper::Get().LoadModel(std::get<const char*>(payload->data));
}

void Widget_Scene::ShowTransformGizmo()
{
	if (Editor_Helper::Get().select_actor.expired()) return;

	auto camera = renderer->GetCamera();
	auto transform = Editor_Helper::Get().select_actor.lock()->GetTransform();

	if (!camera || !transform) return;

	static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(87)) //w
		operation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69)) //e
		operation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) //r
		operation = ImGuizmo::SCALE;

	auto offset = renderer->GetEditorOffset();
	auto size = renderer->GetResolution();
	auto view = camera->GetViewMatrix().Transpose();
	auto proj = camera->GetProjectionMatrix().Transpose();
	auto world = transform->GetWorldMatrix().Transpose();
	
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(offset.x, offset.y, size.x, size.y);
	ImGuizmo::Manipulate(view, proj, operation, mode, world);

	world.Transpose();
	transform->SetTranslation(world.GetTranslation());
	transform->SetRotation(world.GetRotation());
	transform->SetScale(world.GetScale());
}

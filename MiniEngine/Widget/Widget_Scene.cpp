#include "stdafx.h"
#include "Widget_Scene.h"

Widget_Scene::Widget_Scene(Context * context)
    : IWidget(context)
{
    title = "Scene";
	window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse; //���콺 ������ �� �ڵ����� ��ũ�� �Ǵ� �� ����

	timer = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();

	///////////////////////////////Test//////////////////////////////////
}

void Widget_Scene::Render()
{
	ShowFrame();
}

void Widget_Scene::ShowFrame()
{
	auto frame_position_x = static_cast<uint>(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x); //â ���� ���콺 ��ġ�̹Ƿ� window �������� ���ؾ� ���� ��ġ ����
	auto frame_position_y = static_cast<uint>(ImGui::GetCursorPos().y + ImGui::GetWindowPos().y); 

	auto frame_width = static_cast<uint>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto frame_height = static_cast<uint>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	//�� �����ϰ� �Ϸ��� �Ѱ踦 ��������.(0�̸� ������ �ʰ� �����̻� �Ѿ�� overflow)
	frame_width -= frame_width % 2 != 0 ? 1 : 0; //�⺻������ �̹��� ����� 2�� ����̹Ƿ� �̿� �����ֱ� ���� ó�� 
	frame_height -= frame_height % 2 != 0 ? 1 : 0;

	renderer->SetEditorOffset(Vector2(static_cast<float>(frame_position_x), static_cast<float>(frame_position_y))); //���� ��ŷ�� ���

	if (frame_counter >= 0.1f) //â�� ũ�Ⱑ �ٲ�� ���� ũ�⵵ �ٲٴµ� �̿� �ּҽð��� �༭ ���� ����
	{
		renderer->SetResolution(frame_width, frame_height);
		frame_counter = 0.0f;
	}
	frame_counter += timer->GetDeltaTimeSec();

	ImGui::Image( //���
		renderer->GetFrameResource(),
		ImVec2(static_cast<float>(frame_width), static_cast<float>(frame_height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(1, 1, 1, 1),
		ImColor(255, 0, 0, 255)
	);
}

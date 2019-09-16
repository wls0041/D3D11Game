#include "stdafx.h"
#include "Widget_MenuBar.h"

Widget_MenuBar::Widget_MenuBar(Context * context)
	: IWidget(context)
	, is_show_metrics(false)
	, is_show_style_editor(false)
	, is_show_demo(false)
	, is_show_script_editor(false)
{
}

void Widget_MenuBar::Render()
{
	is_show_script_editor = Editor_Script::Get().IsVisible();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New")) {}
			if (ImGui::MenuItem("Open")) {}

			ImGui::Separator();

			if (ImGui::MenuItem("Save")) {}
			if (ImGui::MenuItem("Save as...")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::MenuItem("Metrics", nullptr, &is_show_metrics);
			ImGui::MenuItem("Style", nullptr, &is_show_style_editor);
			ImGui::MenuItem("Demo", nullptr, &is_show_demo);
			ImGui::MenuItem("Script", nullptr, &is_show_script_editor);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	Editor_Script::Get().SetVisible(is_show_script_editor);

	const auto style_editor = []() {ImGui::Begin("StyleEditor"); ImGui::ShowStyleEditor(); ImGui::End(); };

	if (is_show_metrics)        ImGui::ShowMetricsWindow();
	if (is_show_style_editor)   style_editor();
	if (is_show_demo)           ImGui::ShowDemoWindow();
	if (is_show_script_editor)  Editor_Script::Get().Render();
}

#include "stdafx.h"
#include "Widget_ToolBar.h"

namespace ToolBar_Data
{
	static std::vector<std::string> debug_buffers
	{
		"None",
		"Albedo",
		"Normal",
		"Material",
		"Diffuse",
		"Specular",
		"Velocity",
		"Depth",
		"SSAO",
		"SSR",
		"Bloom",
		"Shadows",
	};
	static std::string select_debug_buffer = debug_buffers[0];
	static int select_index = 0;
}

Widget_ToolBar::Widget_ToolBar(Context * context)
    : IWidget(context)
	, render_option_alpha(1.0f)
	, is_show_render_option(false)
{
    title = "ToolBar";
    window_flags |=
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoDocking;
}

void Widget_ToolBar::Begin()
{
    auto& ctx = *GImGui;
    ctx.NextWindowData.MenuBarOffsetMinVal = ImVec2
    (
        ctx.Style.DisplaySafeAreaPadding.x,
        ImMax(ctx.Style.DisplaySafeAreaPadding.y - ctx.Style.FramePadding.y, 0.0f)
    );

    ImGui::SetNextWindowPos(ImVec2(ctx.Viewports[0]->Pos.x, ctx.Viewports[0]->Pos.y + 23.9f));
    ImGui::SetNextWindowSize
    (
        ImVec2
        (
            ctx.Viewports[0]->Size.x,
            ctx.NextWindowData.MenuBarOffsetMinVal.y + ctx.FontBaseSize + ctx.Style.FramePadding.y + 20.0f
        )
    );

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 5.0f));
    ImGui::Begin(title.c_str(), &is_visible, window_flags);
}

void Widget_ToolBar::Render()
{
    //Play Button
    ImGui::SameLine();
    ImGui::PushStyleColor
    (
        ImGuiCol_Button,
        ImGui::GetStyle().Colors[Engine::IsFlagsEnabled(EngineFlags_Game) ? ImGuiCol_ButtonActive : ImGuiCol_Button]
    );

    if (Icon_Provider::Get().ImageButton(IconType::Button_Play, 20.0f)) Engine::FlagsToggle(EngineFlags_Game);

    ImGui::PopStyleColor();

    //Option Button
	ImGui::SameLine();
	ImGui::PushStyleColor
	(
		ImGuiCol_Button,
		ImGui::GetStyle().Colors[is_show_render_option ? ImGuiCol_ButtonActive : ImGuiCol_Button]
	);

	if (Icon_Provider::Get().ImageButton(IconType::Button_Option, 20.0f)) is_show_render_option = true;

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();

	if (is_show_render_option) ShowRenderOptions();
}

void Widget_ToolBar::ShowRenderOptions()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, render_option_alpha);
	ImGui::Begin("Render Options", &is_show_render_option, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking);
	{
		ImGui::TextUnformatted("Opacity");
		ImGui::SameLine();
		ImGui::SliderFloat("##Opacity", &render_option_alpha, 0.1f, 1.0f, "%.1f");

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//Debug Buffers
			if (ImGui::BeginCombo("Buffers", ToolBar_Data::select_debug_buffer.c_str()))
			{
				for (auto i = 0; i < ToolBar_Data::debug_buffers.size(); i++)
				{
					const auto is_selected = ToolBar_Data::select_debug_buffer == ToolBar_Data::debug_buffers[i];
					if (ImGui::Selectable(ToolBar_Data::debug_buffers[i].c_str(), is_selected))
					{
						ToolBar_Data::select_debug_buffer = ToolBar_Data::debug_buffers[i];
						ToolBar_Data::select_index = static_cast<int>(i);
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			Editor_Helper::Get().renderer->SetDebugBufferType(static_cast<RenderBufferType>(ToolBar_Data::select_index));
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

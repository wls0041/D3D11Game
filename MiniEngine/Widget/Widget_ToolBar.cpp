#include "stdafx.h"
#include "Widget_ToolBar.h"

Widget_ToolBar::Widget_ToolBar(Context * context)
    : IWidget(context)
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
	ImGui::PopStyleVar();
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

    if (Icon_Provider::Get().ImageButton(IconType::Button_Play, 20.0f))
        Engine::FlagsToggle(EngineFlags_Game);

    ImGui::PopStyleColor();

    //Option Button

}

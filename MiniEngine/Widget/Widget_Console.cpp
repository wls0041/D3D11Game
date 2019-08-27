#include "stdafx.h"
#include "Widget_Console.h"

namespace Console
{
    static bool is_update_scroll = false;
    static std::vector<ImVec4> log_color
    {
        ImVec4(0.76f, 0.77f, 0.8f, 1.0f), // Info
        ImVec4(0.75f, 0.75f, 0.0f, 1.0f), // Warning
        ImVec4(0.75f, 0.0f,  0.0f, 1.0f), // Error
    };
    static ImGuiTextFilter log_filter;
}

Widget_Console::Widget_Console(Context * context)
    : IWidget(context)
    , max_log_count(500)
    , is_show_info(true)
    , is_show_warning(true)
    , is_show_error(true)
{
    title = "Console";

    logger = std::make_shared<Engine_Logger>();
    logger->SetCallback([this](const Log_Pair& log_pair) { AddLog(log_pair); });

    Log::SetLogger(logger);

    LOG_TO_FILE(false);
}

void Widget_Console::Render()
{
    if (ImGui::Button("Clear"))
        ClearLog();

    ImGui::SameLine();

    const auto display_button = [this](const IconType& type, bool* toggle)
    {
        ImGui::PushStyleColor
        (
            ImGuiCol_Button,
            ImGui::GetStyle().Colors[*toggle ? ImGuiCol_ButtonActive : ImGuiCol_Button]
        );

		if (Icon_Provider::Get().ImageButton(type, 15.0f))
        {
            *toggle = !(*toggle);
            Console::is_update_scroll = true;
        }

        ImGui::PopStyleColor();
        ImGui::SameLine();
    };

    display_button(IconType::Console_Info, &is_show_info);
    display_button(IconType::Console_Warning, &is_show_warning);
    display_button(IconType::Console_Error, &is_show_error);

    Console::log_filter.Draw("Filter", -100.0f);
    ImGui::Separator();

    ImGui::BeginChild("Log", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        for (const auto& log : logs)
        {
            if (!Console::log_filter.PassFilter(log.text.c_str()))
                continue;

            if ((log.type == 0 && is_show_info) || (log.type == 1 && is_show_warning) || (log.type == 2 && is_show_error))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Console::log_color[log.type]);
                ImGui::TextUnformatted(log.text.c_str());
                ImGui::PopStyleColor();
            }
        }

        if (Console::is_update_scroll)
        {
            ImGui::SetScrollHereY();
            Console::is_update_scroll = false;
        }
    }
    ImGui::EndChild();
}

void Widget_Console::AddLog(const Log_Pair & log_pair)
{
    logs.push_back(log_pair);

    if (static_cast<uint>(logs.size()) > max_log_count)
        logs.pop_front();

    Console::is_update_scroll = true;
}

void Widget_Console::ClearLog()
{
    logs.clear();
    logs.shrink_to_fit();
}

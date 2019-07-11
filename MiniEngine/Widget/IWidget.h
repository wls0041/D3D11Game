#pragma once
#include "stdafx.h"

class IWidget
{
public:
    IWidget(class Context* context)
        : context(context)
        , title("")
        , x_min(0.0f)
        , x_max(std::numeric_limits<float>::max())
        , y_min(0.0f)
        , y_max(std::numeric_limits<float>::max())
        , height(0.0f)
        , window_flags(ImGuiWindowFlags_NoCollapse)
        , is_visible(true)
    {}
    virtual ~IWidget() = default;

    virtual void Begin()
    {
        if (!is_visible)
            return;

        ImGui::SetNextWindowSize(ImVec2(x_min, y_min), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(x_min, y_min), ImVec2(x_max, y_max));
        ImGui::Begin(title.c_str(), &is_visible, window_flags);
    }

    virtual void Render() = 0;
    
    virtual void End()
    {
        if (!is_visible)
            return;

        height = ImGui::GetWindowHeight();
        ImGui::End();
    }

    auto GetHeight() const -> const float& { return height; }
    
    auto IsVisible() const -> const bool& { return is_visible; }
    void SetVisible(const bool& is_visible) { this->is_visible = is_visible; }

protected:
    class Context* context;
    std::string title;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float height;
    int window_flags;
    bool is_visible;
};
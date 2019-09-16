#pragma once
#include "IWidget.h"

class Widget_MenuBar final : public IWidget
{
public:
    Widget_MenuBar(class Context* context);
    ~Widget_MenuBar() = default;

    void Begin() override {}
    void Render() override;
    void End() override {}

private:
    bool is_show_metrics;
    bool is_show_style_editor;
    bool is_show_demo;
	bool is_show_script_editor;
};
#pragma once
#include "IWidget.h"

class Widget_ToolBar final : public IWidget
{
public:
    Widget_ToolBar(class Context* context);
    ~Widget_ToolBar() = default;

    void Begin() override;
    void Render() override;

private:
	void ShowRenderOptions();

private:
	float render_option_alpha;
	bool is_show_render_option;
};
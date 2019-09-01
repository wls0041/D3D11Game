#pragma once
#include "IWidget.h"

class Widget_ToolBar final : public IWidget
{
public:
    Widget_ToolBar(class Context* context);
    ~Widget_ToolBar() = default;

    void Begin() override;
    void Render() override;

};
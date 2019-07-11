#pragma once
#include "IWidget.h"

class Widget_Console final : public IWidget
{
public:
    Widget_Console(class Context* context);
    ~Widget_Console() = default;

    void Render() override;

private:

};
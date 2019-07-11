#pragma once
#include "IWidget.h"

class Widget_Scene final : public IWidget
{
public:
    Widget_Scene(class Context* context);
    ~Widget_Scene() = default;

    void Render() override;

private:

};
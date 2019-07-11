#pragma once
#include "IWidget.h"

class Widget_Project final : public IWidget
{
public:
    Widget_Project(class Context* context);
    ~Widget_Project() = default;

    void Render() override;

private:

};
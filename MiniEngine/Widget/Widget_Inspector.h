#pragma once
#include "IWidget.h"

class Widget_Inspector final : public IWidget
{
public:
    Widget_Inspector(class Context* context);
    ~Widget_Inspector() = default;

    void Render() override;


private:
	void ShowTransform(std::shared_ptr<class Transform> &transform) const;
};
#pragma once
#include "IWidget.h"

class Widget_Project final : public IWidget
{
public:
    Widget_Project(class Context* context);
    ~Widget_Project();

    void Render() override;

private:

private:
	std::unique_ptr<class FileDialog> file_dialog_view;
	std::unique_ptr<class FileDialog> file_dialog_load;
};
#include "stdafx.h"
#include "Widget_Project.h"
#include "../Helper/FileDialog.h"

namespace Project
{
	static bool is_show_file_dialog_view = true;
	static bool is_show_file_dialog_load = false;
	static std::string double_clicked_path;
}

Widget_Project::Widget_Project(Context * context)
    : IWidget(context)
{
    title = "Project";
	window_flags |= ImGuiWindowFlags_NoScrollbar;

	file_dialog_view = std::make_unique<FileDialog>(context, FileDialogType::Browser, FileDialogOperation::Load, FileDialogFilter::All, false);
}

Widget_Project::~Widget_Project() = default;

void Widget_Project::Render()
{
	if (!is_visible) return;
	if (ImGui::Button("Import")) { }//Project::is_show_file_dialog_load = true;

	ImGui::SameLine();

	file_dialog_view->Show(&Project::is_show_file_dialog_view);

	//TODO :
	
}

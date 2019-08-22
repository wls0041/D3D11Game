#include "stdafx.h"
#include "Widget_ProgressBar.h"
#include "Resource/ProgressReport.h"

Widget_ProgressBar::Widget_ProgressBar(Context * context)
	: IWidget(context)
{
	title = "Hold on...";
	x_min = 500.0f;
	y_min = 83.0f;
	window_flags |= ImGuiColumnsFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;
	is_visible = false;
}

void Widget_ProgressBar::Begin()
{
	auto &report = ProgressReport::Get();
	auto is_model_loading = report.IsLoading(ProgressReport::Model);

	if (is_model_loading) {
		status = report.GetStatus(ProgressReport::Model);
		progress = report.GetPercentage(ProgressReport::Model);
	}

	is_visible = is_model_loading;

	IWidget::Begin();
}

void Widget_ProgressBar::Render()
{
	if (!is_visible) return;

	ImGui::SetWindowFocus();
	ImGui::PushItemWidth(500.0f - ImGui::GetStyle().WindowPadding.x * 2.0f);
	ImGui::ProgressBar(progress, ImVec2(0, 0));
	ImGui::TextUnformatted(status.c_str());
	ImGui::PopItemWidth();
}

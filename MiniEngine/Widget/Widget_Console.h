#pragma once
#include "IWidget.h"
#include "./Log/Engine_Logger.h"

class Widget_Console final : public IWidget
{
public:
    Widget_Console(class Context* context);
    ~Widget_Console() = default;

    void Render() override;

	void AddLog(const Log_Pair &log_pair);
	void ClearLog();
private:
	std::shared_ptr<Engine_Logger> logger;
	std::deque<Log_Pair> logs;
	uint max_log_count;
	bool is_show_info;
	bool is_show_warning;
	bool is_show_error;
};
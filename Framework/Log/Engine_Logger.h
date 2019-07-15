#pragma once
#include "Framework.h"
#include "ILogger.h"

struct Log_Pair {
	std::string text;
	int type;
};

class Engine_Logger final : public ILogger
{
public:
	typedef std::function<void(const Log_Pair&)> log_function;

public:
	void SetCallBack(log_function &&func) {
		function = std::forward<log_function>(func);
	}

	void Log(const std::string &text, const uint &type) override {
		Log_Pair log_pair;
		log_pair.text = text;
		log_pair.type = type;

		//원래는 null체크 필요
		function(log_pair);
	}

private:
	log_function function;
};
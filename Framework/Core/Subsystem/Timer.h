#pragma once
#include "ISubsystem.h"

class Timer final : public ISubsystem
{
public:
	Timer(class Context *context);
	~Timer() = default;

	const bool Initialize() override;
	
	auto GetDeltaTimeMs() -> const float { return static_cast<float>(delta_time_ms); }
	auto GetDeltaTimeSec() -> const float { return static_cast<float>(delta_time_ms) / 1000.0f; }

	void Update();
	
private:
	double delta_time_ms;

	std::chrono::high_resolution_clock::time_point prev_time;
};
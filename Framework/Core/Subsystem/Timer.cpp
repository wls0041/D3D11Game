#include "Framework.h"
#include "Timer.h"

Timer::Timer(Context * context)
	: ISubsystem(context)
	, delta_time_ms(0.0f)
{
	SUBSCRIBE_TO_EVENT(EventType::Update, EVENT_HANDLER(Update));
}

const bool Timer::Initialize()
{
	prev_time = std::chrono::high_resolution_clock::now();

	return true;
}

void Timer::Update()
{
	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> ms = current_time - prev_time;
	prev_time = current_time;

	delta_time_ms = ms.count();
}

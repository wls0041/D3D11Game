#include "Framework.h"
#include "Thread.h"

Thread::Thread(Context * context)
	: ISubsystem(context)
	, is_stopping(false)
{
	thread_count = std::thread::hardware_concurrency() - 1; // -1 --> 지금 쓰고 있는 main Thread 제외
}

Thread::~Thread()
{
	std::unique_lock<std::mutex> lock(task_mutex);
	{
		is_stopping = true;
	}
	lock.unlock();

	condition_var.notify_all();

	for (auto& thread : threads)
		thread.join(); //작업이 끝날 때까지 대기. thread들이 진행되면서 작업이 완료되기 전에 main code가 종료되면 안되기 때문에 thread들을 기다려줌.

	threads.clear();
	threads.shrink_to_fit();
}

const bool Thread::Initialize() //풀링. 미리 할당해놓고 주고 회수하는 것을 반복. 이를 통해 시간을 단축.
{
	for (uint i = 0; i < thread_count; i++)
		threads.emplace_back(std::thread(&Thread::Invoke, this));

	return true;
}

void Thread::Invoke()
{
	std::shared_ptr<Task> task;

	while (true)
	{
		std::unique_lock<std::mutex> lock(task_mutex);
		{
			condition_var.wait(lock, [this]() { return !tasks.empty() || is_stopping; });
			if (is_stopping && tasks.empty()) return;

			task = tasks.front();
			tasks.pop();
		}
		lock.unlock();

		task->Execute();
	}
}

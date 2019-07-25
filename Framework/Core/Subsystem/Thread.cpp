#include "Framework.h"
#include "Thread.h"

Thread::Thread(Context * context)
	: ISubsystem(context)
	, is_stopping(false)
{
	thread_count = std::thread::hardware_concurrency() - 1; // -1 --> ���� ���� �ִ� main Thread ����
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
		thread.join(); //�۾��� ���� ������ ���. thread���� ����Ǹ鼭 �۾��� �Ϸ�Ǳ� ���� main code�� ����Ǹ� �ȵǱ� ������ thread���� ��ٷ���.

	threads.clear();
	threads.shrink_to_fit();
}

const bool Thread::Initialize() //Ǯ��. �̸� �Ҵ��س��� �ְ� ȸ���ϴ� ���� �ݺ�. �̸� ���� �ð��� ����.
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

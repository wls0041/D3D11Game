#pragma once
#include "ISubsystem.h"

//Task(�۾���) --> Process --> Thread
//						   --> Thread
//             --> Process --> Thread
//			   			   --> Thread

//CPU�� �⺻������ ���ı����� �Դٰ�����. ������ �� ó�� �������� ����� �ƴ�.
//GPU�� ���ı����� ����Ǿ� ��¥ ������� �� �� ����.

//Process�� ���� ������
//Thread�� Stack ������ Heap, Data, Code�� ����
// --> ���� ������ Process�� ���ؼ� �� �� �ִ� ��. Thread������ ���� Stack. ������������ �� ���� ��.

//Storage Class - ���η�

//Stack
//Haap
//Data
//Code

//Ram �޸𸮿� �������� ����Ǵµ� ��� ������ ��Ģ�� ����.
//ũ�⿡ ���� �켱������ ���µ� �̸� �������°� ���η�

class Task final
{
public:
	typedef std::function<void()> Process;

public:
	Task(Process&& process)
	{
		this->process = std::forward<Process>(process);
	}
	~Task() = default;

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;

	void Execute()
	{
		process();
	}

private:
	Process process;
};

class Thread final : public ISubsystem
{
public:
	Thread(class Context* context);
	~Thread();

	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	const bool Initialize() override;//Ǯ��. �̸� �Ҵ��س��� �ְ� ȸ���ϴ� ���� �ݺ�. �̸� ���� �ð��� ����.

	void Invoke();

	template <typename Process>
	void AddTask(Process&& process);

private:
	std::vector<std::thread> threads;
	std::queue<std::shared_ptr<Task>> tasks;
	std::mutex task_mutex;
	std::condition_variable condition_var; //���Ǻ���. �������� ����� ���. �긦 ���� lock_guard��� �Ұ�. unique_lock�� ����ؾ� ��.
	uint thread_count;
	bool is_stopping;
};

template<typename Process>
inline void Thread::AddTask(Process && process)
{
	if (threads.empty())
	{
		LOG_WARNING("No available threads, function will execute in the same thread");
		process();
		return;
	}

	std::unique_lock<std::mutex> lock(task_mutex); //lock_guard�� �޸� ���� Ÿ�̹��� ���� �� ����
	{
		tasks.push(std::make_shared<Task>(std::bind(std::forward<Process>(process))));
	}
	lock.unlock();

	condition_var.notify_one(); //�۾� ���� �˷��� ������ �ϳ��� ����
}

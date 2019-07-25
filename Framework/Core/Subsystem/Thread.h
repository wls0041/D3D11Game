#pragma once
#include "ISubsystem.h"

//Task(작업대) --> Process --> Thread
//						   --> Thread
//             --> Process --> Thread
//			   			   --> Thread

//CPU는 기본적으로 직렬구조라 왔다갔다함. 병렬인 것 처럼 보이지만 사실은 아님.
//GPU는 병렬구조로 설계되어 진짜 쓰레드라 할 수 있음.

//Process는 서로 독립적
//Thread는 Stack 제외한 Heap, Data, Code를 공유
// --> 같이 연관된 Process를 통해서 알 수 있는 것. Thread각각이 가진 Stack. 지역변수같은 건 서로 모름.

//Storage Class - 기억부류

//Stack
//Haap
//Data
//Code

//Ram 메모리에 랜덤으로 저장되는데 사실 일정한 규칙이 있음.
//크기에 따른 우선순위를 갖는데 이를 구분짓는게 기억부류

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

	const bool Initialize() override;//풀링. 미리 할당해놓고 주고 회수하는 것을 반복. 이를 통해 시간을 단축.

	void Invoke();

	template <typename Process>
	void AddTask(Process&& process);

private:
	std::vector<std::thread> threads;
	std::queue<std::shared_ptr<Task>> tasks;
	std::mutex task_mutex;
	std::condition_variable condition_var; //조건변수. 쓰레드의 통신을 담당. 얘를 쓰면 lock_guard사용 불가. unique_lock을 사용해야 함.
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

	std::unique_lock<std::mutex> lock(task_mutex); //lock_guard와 달리 묶는 타이밍을 정할 수 있음
	{
		tasks.push(std::make_shared<Task>(std::bind(std::forward<Process>(process))));
	}
	lock.unlock();

	condition_var.notify_one(); //작업 들어감을 알려서 쓰레드 하나를 깨움
}

#pragma once
#include "Framework.h"

enum class EventType : uint
{
	Frame_Start,
	Frame_End,
	Update,
	Render,
};

class EventSystem final
{
public:
	typedef std::function<void()> subscriber;

public:
	static EventSystem& Get()
	{
		static EventSystem instance;
		return instance;
	}

	//�����Լ��� �ƴ� �̻� bind�Լ��� ����� ��. �װ� ������ ������ �ٷ� ���ٷ� �Ϸ��� r���� ���.
	void Subscribe(const EventType& event_type, subscriber&& func)//Rvalue���� ���ΰ� ���θ� �� �޴� ���� �ƴϳ� �ӵ��� ����.
	{
		subscriber_groups[event_type].push_back(std::forward<subscriber>(func));//move��� �Լ��� ����.
	}

	void Unscribe(const EventType& event_type, subscriber&& function)
	{
		const size_t function_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&function));
	
		auto &subscriber_group = subscriber_groups[event_type];
		
		for (auto iter = subscriber_group.begin(); iter != subscriber_group.end();) 
		{
			const size_t subscriber_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&(*iter))); //1byte������ ���ϱ� ���� ĳ����
			if (subscriber_address == function_address) {
				iter = subscriber_group.erase(iter);
				return;
			}
		}
	}

	void Fire(const EventType& event_type)
	{
		if (subscriber_groups.find(event_type) == subscriber_groups.end()) return;

		for (auto subscriber : subscriber_groups[event_type])
			subscriber();
	}

	void Clear()
	{
		subscriber_groups.clear();
	}

private:
	EventSystem() = default;

private:
	std::map<EventType, std::vector<subscriber>> subscriber_groups;


};

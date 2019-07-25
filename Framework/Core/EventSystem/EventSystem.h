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

	//전역함수가 아닌 이상 bind함수로 묶어야 함. 그게 귀찮기 때문에 바로 람다로 하려고 r참조 사용.
	void Subscribe(const EventType& event_type, subscriber&& func)//Rvalue참조 서로가 서로를 못 받는 것은 아니나 속도가 느림.
	{
		subscriber_groups[event_type].push_back(std::forward<subscriber>(func));//move라는 함수도 있음.
	}

	void Unscribe(const EventType& event_type, subscriber&& function)
	{
		const size_t function_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&function));
	
		auto &subscriber_group = subscriber_groups[event_type];
		
		for (auto iter = subscriber_group.begin(); iter != subscriber_group.end();) 
		{
			const size_t subscriber_address = *reinterpret_cast<size_t*>(reinterpret_cast<char*>(&(*iter))); //1byte단위로 비교하기 위해 캐스팅
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

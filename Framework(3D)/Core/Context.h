#pragma once
#include "Framework.h"
#include "./Subsystem/ISubsystem.h"

class Context final
{
public:
	Context() = default;
	~Context() {}

	template <typename T>
	auto RegisterSubsystem()->T*;

	template <typename T>
	auto GetSubsystem()->T*;

private:
	std::vector<class ISubsystem*> subsystems;
};

template<typename T>
inline auto Context::RegisterSubsystem() -> T *
{
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	return static_cast<T*>(subsystems.emplace_back(new T(this)))); //subsystems의 타입인 isubsytem*으로 반환되기 때문에 명시적캐스팅
}

template<typename T>
inline auto Context::GetSubsystem() -> T *
{
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	for (auto subsystem : subsystems)
	{
		if (typeid(T) == typeid(*subsystem)) return static_cast<T*>(subsystem);
	}
	return nullptr;
}

#pragma once
#include "Framework.h"
#include "./Subsystem/ISubsystem.h"



class Context final
{
public:
    Context() = default;
    ~Context()
    {
		for (int i = static_cast<int>(subsystems.size() - 1); i >= 0; i--) SAFE_DELETE(subsystems[i]); //방법1
		//for (auto iter = subsystems.rbegin(); iter != subsystems.rend(); iter++) SAFE_DELETE(*iter); //방법2
	}

	auto InitializeSubsystems() -> const bool
	{
		for (auto subsystem : subsystems)
		{
			if (!subsystem->Initialize())
			{
				assert(false);
				return false;
			}
		}

		return true;
	}

    template <typename T>
    auto RegisterSubsystem() -> T*;

    template <typename T>
    auto GetSubsystem() -> T*;

private:
    std::vector<class ISubsystem*> subsystems;
};

template<typename T>
inline auto Context::RegisterSubsystem() -> T *
{
    static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
    return static_cast<T*>(subsystems.emplace_back(new T(this)));
}

template<typename T>
inline auto Context::GetSubsystem() -> T *
{
    static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
    for (auto subsystem : subsystems)
    {
        if (typeid(T) == typeid(*subsystem)) //*를 붙이지 않으면 ISubsystem 타입이 나오고 *를 붙여야 파생클래스인 자식클래스가 나옴
            return static_cast<T*>(subsystem);
    }
    return nullptr;
}

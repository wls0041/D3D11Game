#pragma once
#include "Framework.h"
#include "./Subsystem/ISubsystem.h"



class Context final
{
public:
    Context() = default;
    ~Context()
    {

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
    return static_cast<T*>(subsystems.emplace_back(new T(this))));
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

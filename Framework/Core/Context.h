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
        if (typeid(T) == typeid(*subsystem)) //*�� ������ ������ ISubsystem Ÿ���� ������ *�� �ٿ��� �Ļ�Ŭ������ �ڽ�Ŭ������ ����
            return static_cast<T*>(subsystem);
    }
    return nullptr;
}

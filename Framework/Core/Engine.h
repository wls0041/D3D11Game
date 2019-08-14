#pragma once

enum EngineFlags : uint
{
	EngineFlags_Update	= 1U << 0,
	EngineFlags_Render	= 1U << 1,
	EngineFlags_Game	= 1U << 2,
	EngineFlags_Physics = 1U << 3,
};

class Engine final
{
public:
    static auto GetEngineFlags()->const uint& { return engine_flags; }
    static void SetEngineFlags(const uint& flags) { engine_flags = flags; } //static������ this��� x
    static void FlagsEnable(const EngineFlags& flag) { engine_flags |= flag; }
    static void FlagsDisable(const EngineFlags& flag) { engine_flags &= ~flag; }
    static void FlagsToggle(const EngineFlags& flag) { IsFlagsEnabled(flag) ? FlagsDisable(flag) : FlagsEnable(flag); }
    static auto IsFlagsEnabled(const EngineFlags& flag)->const bool { return (engine_flags & flag) > 0UL; } //0���� ũ�� = �� �ϳ��� �����ִ�. ��� ���������� 0���� �����Ƿ�

public:
    Engine();
    ~Engine();

    auto GetContext()  const -> class Context* { return context; }

    void Update();

private:
	static uint engine_flags;
    class Context* context;
};
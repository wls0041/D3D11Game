#pragma once

class Engine final
{
public:
    Engine();
    ~Engine();

    auto GetContext()  const -> class Context* { return context; }

    void Update();
    void Render();

private:
    class Context* context;
	class Timer *timer;
    class Input* input;
    class Scene* scene;
};
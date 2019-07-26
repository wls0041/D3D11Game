#pragma once

class Engine final
{
public:
    Engine();
    ~Engine();

    auto GetContext()  const -> class Context* { return context; }

    void Update();

private:
    class Context* context;
};
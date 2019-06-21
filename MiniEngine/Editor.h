#pragma once

class Editor final
{
public:
    Editor();
    ~Editor();

    void Update();
    void Render();

private:
    class Engine* engine;
    class Context* context;
    class Graphics* graphics;
};
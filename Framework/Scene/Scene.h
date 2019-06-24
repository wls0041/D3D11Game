#pragma once

class Scene final
{
public:
    Scene(class Context* context);
    ~Scene();

    void Update();
    void Render();

private:
    class Context* context;
    class Graphics* graphics;
    class Camera* camera;
    class ConstantBuffer* camera_buffer;
    
    class Terrain* terrain;
};
#pragma once

class Scene final
{
public:
	Scene(class Context* context);
	~Scene();

	void Update();
	void Render();

private:
};
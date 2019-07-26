#pragma once
#include "IComponent.h"

class Transform final : public IComponent
{
public:
	Transform(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Transform() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

private:
	Vector3 local_scale;
	Vector3 local_translation;
	Quaternion local_rotation;

	Matrix local;
	Matrix world;

	Transform *parent;
	std::vector<Transform*> childs;
};
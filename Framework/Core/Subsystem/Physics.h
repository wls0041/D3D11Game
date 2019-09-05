#pragma once
#include "ISubsystem.h"

class Physics final : public ISubsystem
{
public:
	Physics(class Context *context);
	~Physics();

	const bool Initialize() override;

	auto GetGravity() const -> const Vector3; //�ٽ�1
	auto GetWorld() const -> class btDiscreteDynamicsWorld* { return world; } //�ٽ�2
	auto GetPhysicsDebugDraw() const -> class PhysicsDebugDraw* { return debug_draw; } //�ٽ�3
	auto IsSimulating() const -> const bool& { return is_simulating; }

	void Update();

private: 
	class Timer *timer;
	class Renderer *renderer;

	class btBroadphaseInterface *broadphase;
	class btDefaultCollisionConfiguration *collision_configuration; //�浹 �⺻
	class btCollisionDispatcher *dispatcher;
	class btConstraintSolver *constraint_solver; //�浹 ����
	class btDiscreteDynamicsWorld *world; //�ٽ�. �浹 ������ ���� ���⼭ ����
	class PhysicsDebugDraw *debug_draw;

	int max_sub_step;
	bool is_simulating;
};
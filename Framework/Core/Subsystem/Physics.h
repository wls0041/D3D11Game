#pragma once
#include "ISubsystem.h"

class Physics final : public ISubsystem
{
public:
	Physics(class Context *context);
	~Physics();

	const bool Initialize() override;

	auto GetGravity() const -> const Vector3; //«ŸΩ…1
	auto GetWorld() const -> class btDiscreteDynamicsWorld* { return world; } //«ŸΩ…2
	auto GetPhysicsDebugDraw() const -> class PhysicsDebugDraw* { return debug_draw; } //«ŸΩ…3
	auto IsSimulating() const -> const bool& { return is_simulating; }

	void Update();

private: 
	class Timer *timer;
	class Renderer *renderer;

	class btBroadphaseInterface *broadphase;
	class btDefaultCollisionConfiguration *collision_configuration; //√Êµπ ±‚∫ª
	class btCollisionDispatcher *dispatcher;
	class btConstraintSolver *constraint_solver; //√Êµπ ¡¶æ‡
	class btDiscreteDynamicsWorld *world; //«ŸΩ…. √Êµπ ∞®¡ˆµÁ ππµÁ ø©±‚º≠ «ÿ¡‹
	class PhysicsDebugDraw *debug_draw;

	int max_sub_step;
	bool is_simulating;
};
#pragma once

#pragma warning(push, 0)
#include <LinearMath/btIDebugDraw.h>
#pragma warning(pop)

class PhysicsDebugDraw final : public btIDebugDraw
{
public:
	PhysicsDebugDraw(class Context* context);
	~PhysicsDebugDraw() = default;

	void drawLine
	(
		const btVector3& from,
		const btVector3& to,
		const btVector3& from_color,
		const btVector3& to_color
	) override;

	void drawLine
	(
		const btVector3& from,
		const btVector3& to,
		const btVector3& color
	) override;

	void drawContactPoint
	(
		const btVector3& point_on_b,
		const btVector3& normal_on_b,
		btScalar distance,
		int life_time,
		const btVector3& color
	) override;

	void reportErrorWarning(const char* error_warning) override;
	void draw3dText(const btVector3& location, const char* text_string) override {}
	void setDebugMode(int debug_mode) override { this->debug_mode = debug_mode; }
	int getDebugMode() const override { return debug_mode; }

private:
	class Renderer* renderer;
	int debug_mode;
};
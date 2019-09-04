#include "Framework.h"
#include "PhysicsDebugDraw.h"
#include "BulletPhysicsHelper.h"

PhysicsDebugDraw::PhysicsDebugDraw(Context * context)
{
	renderer = context->GetSubsystem<Renderer>();
	debug_mode =
		DBG_DrawWireframe |
		DBG_DrawContactPoints |
		DBG_DrawConstraints |
		DBG_DrawConstraintLimits |
		DBG_DrawNormals |
		DBG_DrawFrames;
}

void PhysicsDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & from_color, const btVector3 & to_color)
{
}

void PhysicsDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
}

void PhysicsDebugDraw::drawContactPoint(const btVector3 & point_on_b, const btVector3 & normal_on_b, btScalar distance, int life_time, const btVector3 & color)
{
}

void PhysicsDebugDraw::reportErrorWarning(const char * error_warning)
{
	LOG_ERROR_F("%s", error_warning);
}

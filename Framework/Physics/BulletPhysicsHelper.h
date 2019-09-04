#pragma once
#include "Framework.h"

#pragma warning(push, 0)
#include <LinearMath/btQuaternion.h>
#pragma warning(pop)

namespace BulletPhysicsHelper
{
	inline const Vector3 ToVector3(const btVector3& vec)
	{
		return Vector3(vec.getX(), vec.getY(), vec.getZ());
	}

	inline const Vector4 ToVector4(const btVector4& vec)
	{
		return Vector4(vec.getX(), vec.getY(), vec.getZ(), vec.getW());
	}

	inline const Vector4 ToVector4(const btVector3& vec)
	{
		return Vector4(vec.getX(), vec.getY(), vec.getZ(), 1.0f);
	}

	inline const Quaternion ToQuaternion(const btQuaternion& qut)
	{
		return Quaternion(qut.getX(), qut.getY(), qut.getZ(), qut.getW());
	}

	inline const btVector3 ToBtVector3(const Vector3& vec)
	{
		return btVector3(vec.x, vec.y, vec.z);
	}

	inline const btQuaternion ToBtQuaternion(const Quaternion& qut)
	{
		return btQuaternion(qut.x, qut.y, qut.z, qut.w);
	}
}
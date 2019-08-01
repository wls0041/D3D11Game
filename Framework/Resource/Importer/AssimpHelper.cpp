#include "Framework.h"
#include "AssimpHelper.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

auto AssimpHelper::ToVector2(const aiVector2D & value) -> const Vector2
{
	return Vector2(value.x, value.y);
}

auto AssimpHelper::ToVector2(const aiVector3D & value) -> const Vector2
{
	return Vector2(value.x, value.y);
}

auto AssimpHelper::ToVector3(const aiVector3D & value) -> const Vector3
{
	return Vector3(value.x, value.y, value.z);
}

auto AssimpHelper::ToColor(const aiColor4D & value) -> const Color4
{
	return Color4(value.r, value.g, value.b, value.a);
}

auto AssimpHelper::ToQuaternion(const aiQuaternion & value) -> const Quaternion
{
	return Quaternion(value.x, value.y, value.z, value.w);
}

auto AssimpHelper::ToMatrix(const aiMatrix4x4 & value) -> const Matrix
{
	return Matrix(
		value.a1, value.b1, value.c1, value.d1,
		value.a2, value.b2, value.c2, value.d2,
		value.a3, value.b3, value.c3, value.d3,
		value.a4, value.b4, value.c4, value.d4
	);
}

void AssimpHelper::ComputeNodeCount(aiNode * node, int * count)
{
	if (!node) return;

	*count++;
	for (uint i = 0; i < node->mNumChildren; i++) ComputeNodeCount(node->mChildren[i], count);
}

void AssimpHelper::ComputeActorTransform(aiNode * node, Actor * actor)
{
	if (!node || !actor) return;

	auto matrix = ToMatrix(node->mTransformation);
	actor->GetTransform()->SetLocalScale(matrix.GetScale());
	actor->GetTransform()->SetLocalRotation(matrix.GetRotation());
	actor->GetTransform()->SetLocalTranslation(matrix.GetTranslation());
}

auto AssimpHelper::Try_Multi_Extension() -> const std::string
{
	return std::string();
}

auto AssimpHelper::ValidatePath(const std::string & original_path, const std::string & model_path) -> const std::string
{
	return std::string();
}

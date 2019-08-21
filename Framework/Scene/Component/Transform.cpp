#include "Framework.h"
#include "Transform.h"
#include "../Actor.h"

Transform::Transform(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, local_scale(1.0f)
	, parent(nullptr)
{
}

void Transform::OnStart()
{
}

void Transform::OnUpdate()
{
}

void Transform::OnStop()
{
}

void Transform::SetLocalScale(const Vector3 & scale)
{
	if (this->local_scale == scale) return;

	local_scale.x = (scale.x == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.x;
	local_scale.y = (scale.y == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.y;
	local_scale.z = (scale.z == 0.0f) ? std::numeric_limits<float>::epsilon() : scale.z;

 	UpdateTransform();
}

void Transform::SetLocalRotation(const Quaternion & rotation)
{
	if (this->local_rotation == rotation) return;

	this->local_rotation = rotation;
	UpdateTransform();
}

void Transform::SetLocalTranslation(const Vector3 & translation)
{
	if (this->local_translation == translation) return;

	this->local_translation = translation;
	UpdateTransform();
}

void Transform::SetScale(const Vector3 & scale)
{
	if (GetScale() == scale) return;

	SetLocalScale(HasParent() ? scale / parent->GetScale() : scale);
}

void Transform::SetRotation(const Quaternion & rotation)
{
	if (GetRotation() == rotation) return;

	SetLocalRotation(HasParent() ? rotation * parent->GetRotation().Inverse() : rotation);
}

void Transform::SetTranslation(const Vector3 & translation)
{
	if (GetTranslation() == translation) return;

	SetLocalTranslation(HasParent() ? translation * parent->GetWorldMatrix().Inverse() : translation);
}

auto Transform::GetWorldRotationMatrix() -> const Matrix
{
	return Matrix::RotationQuaternion(GetRotation());
}

auto Transform::GetRight() -> const Vector3
{
	return local_rotation * Vector3::Right;
}

auto Transform::GetUp() -> const Vector3
{
	return local_rotation * Vector3::Up;
}

auto Transform::GetForward() -> const Vector3
{
	return local_rotation * Vector3::Forward;
}

void Transform::SetParent(Transform * transform)
{
}

auto Transform::GetChildFromIndex(const uint & index) -> Transform *
{
	return nullptr;
}

void Transform::AddChild(Transform * child)
{
}

void Transform::DetachChild()
{
}

void Transform::AcquireChild()
{
}

auto Transform::IsDescendant(Transform * transform) const -> const bool
{
	return false;
}

void Transform::UpdateTransform()
{
	Matrix S = Matrix::Scaling(local_scale);
	Matrix R = Matrix::RotationQuaternion(local_rotation);
	Matrix T = Matrix::Translation(local_translation);

	local = S * R * T;

	if (HasParent()) world = local * parent->GetWorldMatrix();
	else world = local;

	for (const auto& child : childs) child->UpdateTransform();
}

void Transform::UpdateConstantBuffer(const Matrix & view_proj)
{
	if (!gpu_buffer)
	{
		gpu_buffer = std::make_shared<ConstantBuffer>(context);
		gpu_buffer->Create<TRANSFORM_DATA>();
	}

	const auto wvp_current = world * view_proj;

	//TODO : 

	auto gpu_data = gpu_buffer->Map<TRANSFORM_DATA>();
	if (!gpu_data) {
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_data->world = cpu_buffer.world = world;
	gpu_data->wvp_current = cpu_buffer.wvp_current = wvp_current;
	gpu_data->wvp_previous = cpu_buffer.wvp_previous = wvp_previous;

	gpu_buffer->Unmap();

	wvp_previous = wvp_current;

}

#include "Framework.h"
#include "Transform.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"

Transform::Transform(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, local_scale(1.0f)
	, parent(nullptr)
{
	ZeroMemory(&cpu_buffer, sizeof(TRANSFORM_DATA));
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

void Transform::SetParent(Transform * new_parent)
{
	if (!new_parent)
	{
		DetatchChild();
		return;
	}

	if (new_parent->GetID() == GetID())
		return;

	if (HasParent())
	{
		if (parent->GetID() == new_parent->GetID())
			return;
	}

	if (new_parent->IsDescendant(this))
	{
		if (HasParent())
		{
			for (const auto& child : childs)
				child->SetParent(parent);
		}
		else
		{
			for (const auto& child : childs)
				child->DetatchChild();
		}
	}

	auto old_parent = parent;
	parent = new_parent;

	if (old_parent)
		old_parent->AcquireChild();

	if (parent)
		parent->AcquireChild();

	UpdateTransform();
}

auto Transform::GetChildFromIndex(const uint & index) -> Transform *
{
	if (!HasChilds())
	{
		LOG_WARNING(GetActorName() + "has no children");
		return nullptr;
	}

	if (index >= GetChildCount())
	{
		LOG_WARNING("There is no child width an index of \"" + std::to_string(index) + "\"");
		return nullptr;
	}

	return childs[index];
}

auto Transform::GetChildFromName(const std::string & name) -> Transform *
{
	for (const auto& child : childs)
	{
		if (child->GetActorName() == name)
			return child;
	}
	return nullptr;
}

void Transform::AddChild(Transform * child)
{
	if (!child)
		return;

	if (child->GetID() == GetID())
		return;

	child->SetParent(this);
}

void Transform::DetatchChild()
{
	if (!HasParent())
		return;

	auto temp_parent = parent;
	parent = nullptr;

	UpdateTransform();
	if (temp_parent)
		temp_parent->AcquireChild();
}

void Transform::AcquireChild()
{
	childs.clear();
	childs.shrink_to_fit();

	auto scene = context->GetSubsystem<SceneManager>()->GetCurrentScene();
	if (scene)
	{
		auto actors = scene->GetAllActors();
		for (const auto& actor : actors)
		{
			if (!actor)
				continue;

			auto child = actor->GetTransform();

			if (!child->HasParent())
				continue;

			if (child->GetParent()->GetID() == GetID())
			{
				childs.emplace_back(child.get());
				child->AcquireChild();
			}
		}
	}
}

auto Transform::IsDescendant(Transform * transform) const -> const bool
{
	auto descendants = transform->GetChilds();
	for (const auto& descendant : descendants)
	{
		if (descendant->GetID() == GetID())
			return true;
	}
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

void Transform::Translate(const Vector3 & delta)
{
	if (HasParent()) {
	auto position = Vector3::TransformCoord(delta, parent->GetWorldMatrix().Inverse());
	SetLocalTranslation(local_translation + position);
	}
	else SetLocalTranslation(local_translation + delta);
}

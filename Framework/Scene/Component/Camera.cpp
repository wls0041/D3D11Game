#include "Framework.h"
#include "Camera.h"
#include "Transform.h"

Camera::Camera(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, projection_type(ProjectionType::Perspective)
	, fov(Math::ToRadian(45))
	, near_plane(0.3f)
	, far_plane(1000.0f)
	, accelation(0.3f)
	, drag(accelation * 0.8f)
	, movement_speed(0, 0, 0)
{
	input = context->GetSubsystem<Input>();
}

void Camera::OnStart()
{
}

void Camera::OnUpdate()
{
	auto rotation = transform->GetRotation().ToEulerAngle();
	auto right = transform->GetRight();
	auto up = transform->GetUp();
	auto forward = transform->GetForward();


	if (input->BtnPress(KeyCode::CLICK_RIGHT))
	{
		if (input->KeyPress(KeyCode::KEY_W))
			movement_speed += forward * accelation;
		else if (input->KeyPress(KeyCode::KEY_S))
			movement_speed -= forward * accelation;

		if (input->KeyPress(KeyCode::KEY_A))
			movement_speed -= right * accelation;
		else if (input->KeyPress(KeyCode::KEY_D))
			movement_speed += right * accelation;

		if (input->KeyPress(KeyCode::KEY_E))
			movement_speed += up * accelation;
		else if (input->KeyPress(KeyCode::KEY_Q))
			movement_speed -= up * accelation;

		transform->Translate(movement_speed);
		movement_speed *= drag;

		auto delta = input->GetMouseMoveValue();

		rotation.x += delta.y * 0.1f;
		rotation.y += delta.x * 0.1f;

		rotation.x = Math::Clamp(rotation.x, -90.0f, 90.0f);

		transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation));
	}

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::OnStop()
{
}

void Camera::UpdateViewMatrix()
{
	auto position = transform->GetTranslation();
	auto up = transform->GetUp();
	auto forward = transform->GetForward();

	view = Matrix::LookAtLH(position, (position + forward), up);
}

void Camera::UpdateProjectionMatrix()
{
	auto resolution = Vector2(Settings::Get().GetWidth(), Settings::Get().GetHeight());

	switch (projection_type)
	{
	case ProjectionType::Perspective:
		proj = Matrix::PerspectiveFovLH(fov, resolution.x / resolution.y, near_plane, far_plane);
		break;

	case ProjectionType::OrthoGraphics:
		proj = Matrix::OrthoLH(resolution.x, resolution.y, near_plane, far_plane);
		break;
	}
}

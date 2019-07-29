#include "Framework.h"
#include "Camera.h"
#include "Transform.h"

Camera::Camera(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, projection_type(ProjectionType::Perspective)
	, fov(Math::ToRadian(45))
	, near_plane(0.3f)
	, far_plane(1000.0f)
{
	input = context->GetSubsystem<Input>();
}

void Camera::OnStart()
{
}

void Camera::OnUpdate()
{
	//if (input->KeyPress(KeyCode::KEY_W))
	//	position += forward;
	//else if (input->KeyPress(KeyCode::KEY_S))
	//	position -= forward;

	//if (input->KeyPress(KeyCode::KEY_A))
	//	position -= right;
	//else if (input->KeyPress(KeyCode::KEY_D))
	//	position += right;

	//if (input->KeyPress(KeyCode::KEY_E))
	//	position += up;
	//else if (input->KeyPress(KeyCode::KEY_Q))
	//	position -= up;

	//if (input->BtnPress(KeyCode::CLICK_RIGHT))
	//{
	//	auto delta = input->GetMouseMoveValue();

	//	rotation.x += delta.y * 0.005f;
	//	rotation.y += delta.x * 0.005f;
	//}

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

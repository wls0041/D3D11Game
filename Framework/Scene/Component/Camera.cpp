#include "Framework.h"
#include "Camera.h"

Camera::Camera(Context * context)
	: context(context)
	, projection_type(ProjectionType::Perspective)
	, fov(Math::ToRadian(45))
	, near_plane(0.3f)
	, far_plane(1000.0f)
	, position(0, 0, -5)
	, rotation(0, 0, 0)
	, right(1, 0, 0)
	, up(0, 1, 0)
	, forward(0, 0, 1)
{
	input = context->GetSubsystem<Input>();
}

Camera::~Camera()
{
}

void Camera::Update()
{
	if (input->KeyPress(KeyCode::KEY_W))
		position += forward;
	else if (input->KeyPress(KeyCode::KEY_S))
		position -= forward;

	if (input->KeyPress(KeyCode::KEY_A))
		position -= right;
	else if (input->KeyPress(KeyCode::KEY_D))
		position += right;

	if (input->KeyPress(KeyCode::KEY_E))
		position += up;
	else if (input->KeyPress(KeyCode::KEY_Q))
		position -= up;

	if (input->BtnPress(KeyCode::CLICK_RIGHT))
	{
		auto delta = input->GetMouseMoveValue();

		rotation.x += delta.y * 0.005f;
		rotation.y += delta.x * 0.005f;
	}

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{

}

void Camera::UpdateProjectionMatrix()
{
	auto resolution = Vector2(Settings::Get().GetWidth(), Settings::Get().GetHeight());

	switch (projection_type)
	{
	case ProjectionType::Perspective:

		break;

	case ProjectionType::OrthoGraphics:
		break;
	}
}

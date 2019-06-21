#include "Framework.h"
#include "Camera.h"

Camera::Camera(Context * context)
    : context(context)
    , projection_type(ProjectionType::Perspective)
    , fov(static_cast<float>(D3DXToRadian(45)))
    , near_plane(0.3f)
    , far_plane(1000.0f)
    , position(0, 0, -5)
    , rotation(0, 0, 0)
    , right(1, 0, 0)
    , up(0, 1, 0)
    , forward(0, 0, 1)
{
    input = context->GetSubsystem<Input>();

    D3DXMatrixIdentity(&view);
    D3DXMatrixIdentity(&proj);
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
    D3DXMATRIX RX, RY, R;
    D3DXMatrixRotationX(&RX, rotation.x);
    D3DXMatrixRotationY(&RY, rotation.y);

    R = RX * RY;

    D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &R);
    D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &R);
    D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &R);

    D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);
}

void Camera::UpdateProjectionMatrix()
{
    auto resolution = D3DXVECTOR2(Settings::Get().GetWidth(), Settings::Get().GetHeight());

    switch (projection_type)
    {
    case ProjectionType::Perspective:
        D3DXMatrixPerspectiveFovLH
        (
            &proj,
            fov,
            resolution.x / resolution.y,
            near_plane,
            far_plane
        );
        break;

    case ProjectionType::OrthoGraphics:
        break;
    }
}

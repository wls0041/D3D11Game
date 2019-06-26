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

// Local -> World -> View -> Projection -> Viewport

void Camera::GetWorldRay(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction)
{
    auto mouse_position = input->GetMousePosition();
    auto resolution     = D3DXVECTOR2(Settings::Get().GetWidth(), Settings::Get().GetHeight());

    D3DXVECTOR2 point;
    point.x = +2.0f * mouse_position.x / resolution.x - 1.0f;
    point.y = -2.0f * mouse_position.y / resolution.y + 1.0f;
    point.x = point.x / proj._11;
    point.y = point.y / proj._22;

    D3DXMATRIX inverse_view;
    D3DXMatrixInverse(&inverse_view, nullptr, &view);

    auto org = position;
    auto dir = D3DXVECTOR3(point.x, point.y, 1.0f);

    D3DXVec3TransformNormal(&dir, &dir, &inverse_view);
    D3DXVec3Normalize(&dir, &dir);

    origin      = org;
    direction   = dir;
}

void Camera::GetLocalRay(D3DXVECTOR3 & origin, D3DXVECTOR3 & direction, const D3DXMATRIX & world)
{
    D3DXVECTOR3 org, dir;
    GetWorldRay(org, dir);

    D3DXMATRIX inverse_world;
    D3DXMatrixInverse(&inverse_world, nullptr, &world);

    D3DXVec3TransformNormal(&dir, &dir, &inverse_world);
    D3DXVec3Normalize(&dir, &dir);

    D3DXVec3TransformCoord(&org, &org, &inverse_world);

    origin      = org;
    direction   = dir;
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

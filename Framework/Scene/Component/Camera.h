#pragma once

enum class ProjectionType : uint
{
    Perspective,
    OrthoGraphics,
};

class Camera final
{
public:
    Camera(class Context* context);
    ~Camera();

    auto GetViewMatrix() const -> const D3DXMATRIX& { return view; }
    auto GetProjectionMatrix() const -> const D3DXMATRIX& { return proj; }

    auto GetProjectionType() const -> const ProjectionType& { return projection_type; }
    void SetProjectionType(const ProjectionType& type) { this->projection_type = type; }

	auto GetPosition() const -> const D3DXVECTOR3& { return position; }
	void SetPosition(const D3DXVECTOR3 &position) { this->position = position; }

    auto GetFOV() const -> const float& { return fov; }
    void SetFOV(const float& fov) { this->fov = fov; }

    auto GetNearPlane() const -> const float& { return near_plane; }
    void SetNearPlane(const float& near_plane) { this->near_plane = near_plane; }

    auto GetFarPlane() const -> const float& { return far_plane; }
    void SetFarPlane(const float& far_plane) { this->far_plane = far_plane; }

    void GetWorldRay(D3DXVECTOR3& origin, D3DXVECTOR3& direction);
    void GetLocalRay(D3DXVECTOR3& origin, D3DXVECTOR3& direction, const D3DXMATRIX& world);

    void Update();

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

private:
    class Context* context;
    class Input* input;

    ProjectionType projection_type;
    float fov;//field of view
    float near_plane;
    float far_plane;

    D3DXMATRIX view;
    D3DXMATRIX proj;

    D3DXVECTOR3 position;
    D3DXVECTOR3 rotation;

    D3DXVECTOR3 right;
    D3DXVECTOR3 up;
    D3DXVECTOR3 forward;
};
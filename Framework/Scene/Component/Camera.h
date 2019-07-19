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

    auto GetViewMatrix() const -> const Matrix& { return view; }
    auto GetProjectionMatrix() const -> const Matrix& { return proj; }

    auto GetProjectionType() const -> const ProjectionType& { return projection_type; }
    void SetProjectionType(const ProjectionType& type) { this->projection_type = type; }

	auto GetPosition() const -> const Vector3& { return position; }
	void SetPosition(const Vector3 &position) { this->position = position; }

    auto GetFOV() const -> const float& { return fov; }
    void SetFOV(const float& fov) { this->fov = fov; }

    auto GetNearPlane() const -> const float& { return near_plane; }
    void SetNearPlane(const float& near_plane) { this->near_plane = near_plane; }

    auto GetFarPlane() const -> const float& { return far_plane; }
    void SetFarPlane(const float& far_plane) { this->far_plane = far_plane; }

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

    Matrix view;
    Matrix proj;

    Vector3 position;
    Vector3 rotation;

    Vector3 right;
    Vector3 up;
    Vector3 forward;
};
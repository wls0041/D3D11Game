#pragma once
#include "IComponent.h"

enum class ProjectionType : uint
{
    Perspective,
    OrthoGraphics,
};

class Camera final : public IComponent
{
public:
	Camera(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Camera() = default;

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

    auto GetViewMatrix() const -> const Matrix& { return view; }
    auto GetProjectionMatrix() const -> const Matrix& { return proj; }

    auto GetProjectionType() const -> const ProjectionType& { return projection_type; }
    void SetProjectionType(const ProjectionType& type) { this->projection_type = type; }

    auto GetFOV() const -> const float& { return fov; }
    void SetFOV(const float& fov) { this->fov = fov; }

    auto GetNearPlane() const -> const float& { return near_plane; }
    void SetNearPlane(const float& near_plane) { this->near_plane = near_plane; }

    auto GetFarPlane() const -> const float& { return far_plane; }
    void SetFarPlane(const float& far_plane) { this->far_plane = far_plane; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

private:
    class Input* input;

    ProjectionType projection_type;
    float fov;//field of view
    float near_plane;
    float far_plane;

    Matrix view;
    Matrix proj;

	////////юс╫ц////////
	float accelation;
	float drag;
	Vector3 movement_speed;
};
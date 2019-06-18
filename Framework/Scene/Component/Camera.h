#pragma once

enum class ProjectionType : uint
{
	Perspective,
	OrthoGraphics,
};

class Camera final
{
public:
	Camera(class Context *context);
	~Camera();

	auto GetViewMatrix() const -> const D3DXMATRIX& { return view; }
	auto GetProjectionMatrix() const -> const D3DXMATRIX& { return proj; }

	auto SetProjectionType() const -> const ProjectionType&  { return projection_type; }
	void SetProjectionType(const ProjectionType &type) { this->projection_type = type; }

	auto SetFOV() const -> const float& { return fov; }
	void SetFOV(const float &fov) { this->fov = fov; }

	auto SetNearPlane() const -> const float& { return near_plane; }
	void SetNearPlane(const float &near_plane) { this->near_plane = near_plane; }

	auto SetFarPlane() const -> const float& { return far_plane; }
	void SetFarPlane(const float &far_plane) { this->far_plane = far_plane; }

	void Update();

private:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

private:
	class Context *context;
	class Input *input;

	ProjectionType projection_type;
	float fov; //field of view. �þ߰�
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
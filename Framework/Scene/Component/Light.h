#pragma once
#include "IComponent.h"

//Cascade

enum class LightType : uint
{
	Directional,
	Point,
	Spot,
};

class Light final : public IComponent
{
public:
	Light
	(
		class Context* context,
		class Actor* actor,
		class Transform* transform
	);
	~Light();

	void OnStart() override;
	void OnUpdate() override;
	void OnStop() override;

	//======================================================================================
	// [Property]
	//======================================================================================
	auto GetDirection() const->Vector3;

	auto GetLightType() const -> const LightType& { return light_type; }
	void SetLightType(const LightType& type);

	auto GetRange() const -> const float& { return range; }
	void SetRange(const float& range);

	auto GetIntensity() const -> const float& { return intensity; }
	void SetIntensity(const float& intensity) { this->intensity = intensity; }

	auto GetAngle() const -> const float& { return angle_radian; }
	void SetAngle(const float& angle);

	auto GetBias() const -> const float& { return bias; }
	void SetBias(const float& bias) { this->bias = bias; }

	auto GetNormalBias() const -> const float& { return normal_bias; }
	void SetNormalBias(const float& normal_bias) { this->normal_bias = normal_bias; }

	auto GetColor() const -> const Color4& { return color; }
	void SetColor(const Color4& color) { this->color = color; }

	auto IsCastShadow() const -> const bool& { return is_cast_shadow; }
	void SetCastShadow(const bool& is_cast_shadow);

	//======================================================================================
	// [Constant Buffer]
	//======================================================================================
	auto GetConstantBuffer() const -> const std::shared_ptr<ConstantBuffer>& { return gpu_buffer; }
	void UpdateConstantBuffer();

private:
	class Renderer* renderer;

	LightType light_type;
	float range;
	float intensity;
	float angle_radian;
	float bias;
	float normal_bias;
	bool is_update;
	bool is_cast_shadow;
	Color4 color;
	Vector3 last_camera_position;
	Vector3 last_light_position;
	Quaternion last_light_rotation;

	std::array<Matrix, 6> views;
	std::array<Matrix, 6> projections;

	std::shared_ptr<ConstantBuffer> gpu_buffer;
};
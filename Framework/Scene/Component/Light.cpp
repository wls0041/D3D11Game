#include "Framework.h"
#include "Light.h"
#include "Transform.h"
#include "Camera.h"

Light::Light(Context * context, Actor * actor, Transform * transform)
	: IComponent(context, actor, transform)
	, light_type(LightType::Point)
	, range(10.0f)
	, intensity(2.0f)
	, angle_radian(0.5f)
	, bias(0.0f)
	, normal_bias(0.0f)
	, color(1.0f, 0.76f, 0.57f, 1.0f)
	, last_camera_position(0, 0, 0)
	, last_light_position(0, 0, 0)
	, last_light_rotation(0, 0, 0, 1)
	, is_cast_shadow(true)
	, is_update(true)
{
	renderer = context->GetSubsystem<Renderer>();
}

Light::~Light()
{
}

void Light::OnStart()
{
}

void Light::OnUpdate()
{
	if (last_light_position != transform->GetTranslation() || last_light_rotation != transform->GetRotation()) {
		last_light_position = transform->GetTranslation();
		last_light_rotation = transform->GetRotation();

		is_update = true;
	}

	if (light_type == LightType::Directional) {
		if (auto camera = renderer->GetCamera()) {
			if (last_camera_position != camera->GetTransform()->GetTranslation()) {
				last_camera_position = camera->GetTransform()->GetTranslation();
				is_update = true;
			}
		}
	}

	if (!is_update) return;
	//TODO :
}

void Light::OnStop()
{
}

auto Light::GetDirection() const -> Vector3
{
	return transform->GetForward();
}

void Light::SetLightType(const LightType & type)
{
	light_type = type;
	is_update = true;
	is_cast_shadow = light_type == LightType::Directional;

	//TODO : Create Shadow Map
}

void Light::SetRange(const float & range)
{
	this->range = Math::Clamp(range, 0.0f, std::numeric_limits<float>::infinity());
}

void Light::SetAngle(const float & angle)
{
	angle_radian = Math::Clamp(angle, 0.0f, 1.0f);
	is_update = true;
}

void Light::SetCastShadow(const bool & is_cast_shadow)
{
	if (this->is_cast_shadow == is_cast_shadow)
		return;

	this->is_cast_shadow = is_cast_shadow;
	//TODO : Create Shadow Map
}

#include "Framework.h"
#include "Vector3.h"
#include "Vector2.h"

const Vector3 Vector3::Zero(0.0f);
const Vector3 Vector3::One(1.0f);
const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::Back(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::Infinity(std::numeric_limits<float>::infinity());
const Vector3 Vector3::NegInfinity(-std::numeric_limits<float>::infinity());

auto Vector3::Distance(const Vector3 & lhs, const Vector3 & rhs) -> const float
{
	auto delta = lhs - rhs;
	return sqrtf(powf(delta.x, 2) + powf(delta.y, 2) + powf(delta.z, 2));
}

auto Vector3::DistanceSq(const Vector3 & lhs, const Vector3 & rhs) -> const float
{
	auto delta = lhs - rhs;
	return powf(delta.x, 2) + powf(delta.y, 2) + powf(delta.z, 2);
}

auto Vector3::Dot(const Vector3 & lhs, const Vector3 & rhs) -> const float
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

auto Vector3::Cross(const Vector3 & lhs, const Vector3 & rhs) -> const Vector3
{
	float num1 = lhs.y * rhs.z - lhs.z * rhs.y;
	float num2 = lhs.z * rhs.x - lhs.x * rhs.z;
	float num3 = lhs.x * rhs.y - lhs.y * rhs.x;

	return Vector3(num1, num2, num3);
}

auto Vector3::TransformCoord() -> const Vector3
{
	return Vector3();
}

auto Vector3::TransformNormal() -> const Vector3
{
	return Vector3();
}

auto Vector3::Normalize(const Vector3 & rhs) -> const Vector3
{
	auto factor = rhs.Length();
	factor = 1.0f / factor;

	return rhs * factor;
}

Vector3::Vector3()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{
}

Vector3::Vector3(const float & x, const float & y, const float & z)
	: x(x)
	, y(y)
	, z(z)
{
}

Vector3::Vector3(const float & value)
	: x(value)
	, y(value)
	, z(value)
{
}

Vector3::Vector3(const Vector3 & rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(rhs.z)
{
}

Vector3::Vector3(const Vector2 & rhs)
	: x(rhs.x)
	, y(rhs.y)
	, z(0.0f)
{
}

auto Vector3::Normalize() const -> const Vector3
{
	auto factor = Length();
	factor = 1.0f / factor;

	return *this * factor;
}

void Vector3::Normalize()
{
	auto factor = Length();
	factor = 1.0f / factor;

	*this *= factor;
}

void Vector3::Floor()
{
	x = floorf(x);
	y = floorf(y);
	z = floorf(z);
}
#pragma once

enum class Intersection : uint
{
	Outside,
	Inside,
	Intersect
};

class Math final
{
public:
	static constexpr float PI = 3.14159265359f; //리터럴 상수 //constexpr 컴파일 타임에 생성, 체크
	static constexpr float PI_2 = 6.28318530718f;
	static constexpr float PI_DIV_2 = 1.57079632679f;
	static constexpr float PI_DIV_4 = 0.78539816339f;
	static constexpr float TO_DEG = 180.0f / PI;
	static constexpr float TO_RAD = PI / 180.0f;

	static auto ToRadian(const float& degree) -> const float{ return degree * TO_RAD; }
	static auto ToDegree(const float& radian) -> const float{ return radian * TO_DEG; }
	static auto Random(const int& min, const int& max) -> const int;
	static auto Random(const float& min, const float& max) -> const float;

	static  auto Cot(const double &x) -> const double { return cos(x) / sin(x); }
	static  auto Cot(const float&x) -> const float{ return cosf(x) / sinf(x); }

	//radian기준 -pi~pi로
	static auto WrapAngle(const float &angle) -> const float 
	{

	}

	template <typename T>
	static const T Clamp(const T& value, const T& min, const T& max)
	{
		return value < min ? min : (value > max ? max : value);
	}

	template <typename T, typename U>
	static constexpr T Lerp(const T &lhs, const T &rhs, const U &t) {
		return lhs * (static_cast<U>(1.0f) - t) + rhs * t;
	}

	template <typename T>
	static const T Max(const T& lhs, const T& rhs)
	{
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T>
	static const T Min(const T& lhs, const T& rhs)
	{
		return lhs < rhs ? lhs : rhs;
	}

	template <typename T>
	static const T Abs(const T& value)
	{
		return value >= 0 ? value : -value;
	}

	template <typename T>
	static constexpr int Sign(const T &value)
	{
		return (T(0) < value) - (value < T(0));
	}

	template <typename T>
	static const bool Equals(const T& lhs, const T& rhs, const T& e = std::numeric_limits<float>::epsilon())
	{
		return lhs + e >= rhs && lhs - e <= rhs;
	}
};
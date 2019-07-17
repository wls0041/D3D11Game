#pragma once

class Vector2 final
{
public:
	static const Vector2 Zero;
	static const Vector2 One;

public:
	Vector2();
	Vector2(const float& x, const float& y);
	Vector2(const float &value); //변환생성자. 이 덕분에 다른 타입을 집어넣어도 알아서 변환해줌. 이 때문에 문제발생 가능성 있음 -> explicit을 함수 앞에 붙이면 명시적 형변환 만이 가능하게 됨
	Vector2(const Vector2 &rhs); //복사생성자
	~Vector2() = default;

	auto Length() const -> const float { return sqrtf(x * x + y * y); }
	auto LengthSq() const -> const float { return x * x + y * y; }
	auto Normalize() const -> const Vector2; //const를 붙이면 상수로 묶여 오버로딩 가능. 묶지 않으면 매개변수가 같으므로 불가능
	void Normalize();

	operator float*() { return &x; }
	operator const float*() const { return &x; }

	Vector2& operator=(const Vector2& rhs) { x = rhs.x; y = rhs.y; return *this; }

	const Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }
	const Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }
	const Vector2 operator*(const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }
	const Vector2 operator/(const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }

	const Vector2 operator+(const float& rhs) const { return Vector2(x + rhs, y + rhs); }
	const Vector2 operator-(const float& rhs) const { return Vector2(x - rhs, y - rhs); }
	const Vector2 operator*(const float& rhs) const { return Vector2(x * rhs, y * rhs); }
	const Vector2 operator/(const float& rhs) const { return Vector2(x / rhs, y / rhs); }

	void operator+=(const Vector2& rhs) { x += rhs.x; y += rhs.y; }
	void operator-=(const Vector2& rhs) { x -= rhs.x; y -= rhs.y; }
	void operator*=(const Vector2& rhs) { x *= rhs.x; y *= rhs.y; }
	void operator/=(const Vector2& rhs) { x /= rhs.x; y /= rhs.y; }

	void operator+=(const float& rhs) { x += rhs; y += rhs; }
	void operator-=(const float& rhs) { x -= rhs; y -= rhs; }
	void operator*=(const float& rhs) { x *= rhs; y *= rhs; }
	void operator/=(const float& rhs) { x /= rhs; y /= rhs; }

	const bool operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }
	const bool operator!=(const Vector2& rhs) const { return x != rhs.x || y != rhs.y; }

public:
	float x;
	float y;
};
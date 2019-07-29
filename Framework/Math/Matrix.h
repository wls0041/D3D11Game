#pragma once

class Matrix final
{
public:
	static const Matrix Identity;

public:
	static const Matrix Scaling(const float& x, const float& y, const float& z);
	static const Matrix Scaling(const class Vector3& s);

	static const Matrix RotationX(const float& radian);
	static const Matrix RotationY(const float& radian);
	static const Matrix RotationZ(const float& radian);

	static const Matrix RotationQuaternion(const class Quaternion& rotation);
	static const class Quaternion RotationMatrixToQuaternion(const Matrix& rotationMat);

	static const Matrix Translation(const float& x, const float& y, const float& z);
	static const Matrix Translation(const class Vector3& t);

	static const Matrix LookAtLH(const class Vector3& eye, const class Vector3& at, const class Vector3& up);//view행렬
	static const Matrix OrthoLH(const float& w, const float& h, const float& zn, const float& zf);//projection행렬
	static const Matrix OrthoOffCenterLH(const float& l, const float& r, const float& b, const float& t, const float& zn, const float& zf);//center를 옮긴 projection행렬
	static const Matrix PerspectiveFovLH(const float& fov, const float& aspect, const float& zn, const float& zf);//원근

	static const Matrix Inverse(const Matrix& rhs);
	static const Matrix Transpose(const Matrix& rhs);

public:
	Matrix();
	Matrix
	(
		const float& _11, const float& _12, const float& _13, const float& _14,
		const float& _21, const float& _22, const float& _23, const float& _24,
		const float& _31, const float& _32, const float& _33, const float& _34,
		const float& _41, const float& _42, const float& _43, const float& _44
	);
	~Matrix() = default;

	const class Vector3 GetScale();
	const class Quaternion GetRotation();
	const class Vector3 GetTranslation();

	const Matrix Inverse() const { return Inverse(*this); }
	const Matrix Transpose() const { return Transpose(*this); }
	void Transpose() { *this = Transpose(*this); }

	void Decompose(class Vector3& scale, class Quaternion& rotation, Vector3& translation);

	void SetIdentity();

	operator float*() { return &_11; }
	operator const float*() const { return &_11; }

	const Vector3 operator*(const Vector3 &rhs) const;
	const Matrix operator*(const Matrix& rhs) const;
	const bool operator==(const Matrix& rhs) const;
	const bool operator!=(const Matrix& rhs) const { return !(*this == rhs); }

public:
	float _11, _21, _31, _41;
	float _12, _22, _32, _42;
	float _13, _23, _33, _43;
	float _14, _24, _34, _44;
};

inline Vector3 operator*(const Vector3 &lhs, const Matrix &rhs) { return rhs * lhs; }
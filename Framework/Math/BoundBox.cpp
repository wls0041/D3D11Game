#include "Framework.h"
#include "BoundBox.h"

BoundBox::BoundBox()
	: min(Vector3::Infinity)
	, max(Vector3::NegInfinity)
{
}

BoundBox::BoundBox(const Vector3 & min, const Vector3 & max)
	: min(min)
	, max(max)
{
}

BoundBox::BoundBox(const std::vector<VertexTextureNormalTangentBlend>& vertices)
	: min(Vector3::Infinity)
	, max(Vector3::NegInfinity)
{
	for (const auto& vertex : vertices)
	{
		min.x = Math::Min(min.x, vertex.position.x);
		min.y = Math::Min(min.y, vertex.position.y);
		min.z = Math::Min(min.z, vertex.position.z);

		max.x = Math::Max(max.x, vertex.position.x);
		max.y = Math::Max(max.y, vertex.position.y);
		max.z = Math::Max(max.z, vertex.position.z);
	}
}

auto BoundBox::IsInside(const Vector3 & point) const -> const Intersection
{
	if (
		point.x < min.x || point.x > max.x ||
		point.y < min.y || point.y > max.y ||
		point.z < min.z || point.z > max.z)
		return Intersection::Outside;
	else
		return Intersection::Inside;
}

auto BoundBox::IsInside(const BoundBox & box) const -> const Intersection
{
	if (
		box.max.x < min.x || box.min.x > max.x ||
		box.max.y < min.y || box.min.y > max.y ||
		box.max.z < min.z || box.min.z > max.z)
		return Intersection::Outside;
	else if (
		box.min.x < min.x || box.max.x > max.x ||
		box.min.y < min.y || box.max.y > max.y ||
		box.min.z < min.z || box.max.z > max.z)
		return Intersection::Intersect;
	else
		return Intersection::Inside;
}

auto BoundBox::Transformed(const Matrix & transform) -> const BoundBox
{
	Vector3 new_center = Vector3::TransformCoord(GetCenter(), transform);

	Vector3 old_edge = GetExtents();
	Vector3 new_edge = Vector3
	(
		Math::Abs(transform._11) * old_edge.x + Math::Abs(transform._21) * old_edge.y + Math::Abs(transform._31) * old_edge.z,
		Math::Abs(transform._12) * old_edge.x + Math::Abs(transform._22) * old_edge.y + Math::Abs(transform._32) * old_edge.z,
		Math::Abs(transform._13) * old_edge.x + Math::Abs(transform._23) * old_edge.y + Math::Abs(transform._33) * old_edge.z
	);

	return BoundBox(new_center - new_edge, new_center + new_edge);
}

void BoundBox::Merge(const BoundBox & box)
{
	min.x = Math::Min(min.x, box.min.x);
	min.y = Math::Min(min.y, box.min.y);
	min.z = Math::Min(min.z, box.min.z);

	max.x = Math::Max(max.x, box.max.x);
	max.y = Math::Max(max.y, box.max.y);
	max.z = Math::Max(max.z, box.max.z);
}

void BoundBox::Undefine()
{
	min.x = std::numeric_limits<float>::min();
	min.y = std::numeric_limits<float>::min();
	min.z = std::numeric_limits<float>::min();

	max.x = std::numeric_limits<float>::max();
	max.y = std::numeric_limits<float>::max();
	max.z = std::numeric_limits<float>::max();
}

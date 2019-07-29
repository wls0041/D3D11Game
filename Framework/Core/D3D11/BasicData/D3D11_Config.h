#pragma once
#include "Framework.h"

static const std::string NOT_ASSIGNED_STR = "N/A";

enum class ShaderScope : uint
{
    Unknown, VS, PS, GS, DS, HS, CS,
};

struct CameraData
{
    Matrix view;
    Matrix projection;
	Vector3 camera_position;
	float padding;
};

struct WorldData
{
    Matrix world;
};

struct BrushData
{
    int brush_type;
    Vector3 brush_position;
    int brush_range;
    Vector3 brush_color;
};

struct MultiData
{
	int paint_type;
	Vector3 index;
};

struct SkinnedData
{
	Matrix skinned_transforms[100];
};

struct MaterialData
{
	float normal_multiplier;
	Vector3 padding;
};
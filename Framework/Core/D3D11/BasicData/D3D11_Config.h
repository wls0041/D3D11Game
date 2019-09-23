#pragma once
#include "Framework.h"

static const std::string NOT_ASSIGNED_STR = "N/A";
static const std::string MODEL_BIN_EXTENSION = ".model";
static const std::string TEXTURE_BIN_EXTENSION = ".texture";
static const std::string MATERIAL_BIN_EXTENSION = ".mat";
static const std::string MESH_BIN_EXTENSION = ".mesh";

enum class ShaderScope : uint
{
    Unknown, Global, VS, PS, GS, DS, HS, CS,
};

struct GLOBAL_DATA final
{
	Matrix world_view_proj;
	Matrix view;
	Matrix proj;
	Matrix proj_ortho;
	Matrix view_proj;
	Matrix view_proj_inverse;
	Matrix view_proj_ortho;
	float camera_near; 
	float camera_far;
	Vector2 resolution;
	Vector3 camera_position; //크기 범위를 안맞추면 데이터가 잘려서 밀려 들어가 제대로 저장되지 않을 수 있음.
	float directional_light_intensity;
};

struct MATERIAL_DATA final
{
	Color4 albedo_color;
	Vector2 tiling;
	Vector2 offset;
	float roughness_coef;
	float metallic_coef;
	float normal_coef;
	float height_coef;
	float shading_mode;

	float padding[3];
};

struct TRANSFORM_DATA final
{
	Matrix world;
	Matrix wvp_current;
	Matrix wvp_previous;
};

struct LIGHT_DATA final
{
	Vector3 color;
	float intensity;
	Vector3 position;
	float range;
	Vector3 direction;
	float angle;
	float bias;
	float normal_bias;
	float padding[2];
};
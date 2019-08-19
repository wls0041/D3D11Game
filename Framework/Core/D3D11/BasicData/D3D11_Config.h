#pragma once
#include "Framework.h"

static const std::string NOT_ASSIGNED_STR = "N/A";
static const std::string TEXTURE_BIN_EXTENSION = ".texture";
static const std::string MATERIAL_BIN_EXTENSION = ".mat";
static const std::string MESH_BIN_EXTENSION = ".mesh";

enum class ShaderScope : uint
{
    Unknown, Global, VS, PS, GS, DS, HS, CS,
};

struct CPU_GLOBAL_DATA final
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

	float padding;
};

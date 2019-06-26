#pragma once
#include "Framework.h"

static const std::string NOT_ASSIGNED_STR = "N/A";

enum class ShaderScope : uint
{
    VS, PS, GS, DS, HS, CS,
};

struct CameraData
{
    D3DXMATRIX view;
    D3DXMATRIX projection;
};

struct WorldData
{
    D3DXMATRIX world;
};

struct BrushData
{
    int brush_type;
    D3DXVECTOR3 brush_position;
    int brush_range;
    D3DXVECTOR3 brush_color;
};
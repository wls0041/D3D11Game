#pragma once
#include "Framework.h"

struct FbxMaterialData
{
    FbxMaterialData() { ZeroMemory(this, sizeof(FbxMaterialData)); }

    float roughness;
    float metallic;
    float normal;
    float height;
    
    std::string albedo_texture_path;
    std::string roughness_texture_path;
    std::string metallic_texture_path;
    std::string normal_texture_path;
    std::string height_texture_path;
    std::string occlusion_texture_path;
    std::string emissive_texture_path;
    std::string mask_texture_path;
};

struct FbxVertexWeight
{
    FbxVertexWeight() { ZeroMemory(this, sizeof(FbxVertexWeight)); }

    uint index;
    double weight;
};

struct FbxCtrlPointData
{
    FbxCtrlPointData() { ZeroMemory(this, sizeof(FbxCtrlPointData)); }
  
    FbxVector4 position;
    std::vector<FbxVertexWeight> vertex_weights;
};

struct FbxBoneData
{
    FbxBoneData() { ZeroMemory(this, sizeof(FbxBoneData)); }

    std::string name;
    int parent_index;
    D3DXMATRIX offset_transform;
    D3DXMATRIX bind_pose_transform;
};

struct FbxMeshData
{
    FbxMeshData() { ZeroMemory(this, sizeof(FbxMeshData)); }

    std::string name;
    D3DXMATRIX transform;
    std::vector<VertexModel> vertices;
    std::vector<uint> indices;
};

struct FbxKeyframeData
{
    FbxKeyframeData() { ZeroMemory(this, sizeof(FbxKeyframeData)); }

    FbxLongLong time;
    D3DXMATRIX transform;
};

struct FbxBoneKeyframeData
{
    FbxBoneKeyframeData() { ZeroMemory(this, sizeof(FbxBoneKeyframeData)); }

    std::string name;
    std::vector<FbxKeyframeData> keyframes;
};

struct FbxSkinnedAnimationData
{
    FbxSkinnedAnimationData() { ZeroMemory(this, sizeof(FbxSkinnedAnimationData)); }

    std::string name;
    int frame_count;
    float frame_rate;
    float duration;
    std::vector<FbxBoneKeyframeData> bone_animations;
};
#pragma once

class ModelImporter final
{
public:
    ModelImporter(class Context* context);
    ~ModelImporter();

    void Load(const std::string& path);

private:
    void ProcessMaterial(FbxScene* scene);
    void ProcessSkeletonHierarchy(FbxScene* scene);
    void ProcessSkeletonHierarchyRecursively
    (
        FbxNode* node,
        const int& depth,
        const int& index,
        const int& parent_index
    );

    void ProcessGeometryRecursively(FbxNode* node);
    void ProcessCtrlPoints(FbxNode* node);
    void ProcessBone(FbxNode* node);
    void ProcessMesh(FbxNode* node);

private:
    class Context* context;
    std::string fbx_name;

    std::vector<struct FbxMaterialData> material_datas;
    std::vector<struct FbxMeshData> mesh_datas;
    std::vector<struct FbxBoneData> bone_datas;
    std::vector<struct FbxSkinnedAnimationData> animation_datas;

    std::unordered_map<uint, struct FbxCtrlPointData> ctrl_point_datas;
};
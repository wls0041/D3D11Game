#pragma once

class ModelImporter final
{
public:
    ModelImporter(class Context* context);
    ~ModelImporter();

    void Load(const std::string& path, class Model** model);
    void ExportFile();

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

	void ProcessAnimation(FbxScene *scene);
	void ProcessAnimationRecursiely(FbxNode *node, const uint &animation_index, const FbxLongLong &start, const FbxLongLong &end);

    void ProcessGeometryRecursively(FbxNode* node);
    void ProcessCtrlPoints(FbxNode* node);
    void ProcessBone(FbxNode* node);
    void ProcessMesh(FbxNode* node);

private:
    auto ReadUV
    (
        FbxMesh* fbx_mesh,
        const int& layer_index,
        const int& polygon_index,
        const int& polygon_vertex_index,
        const int& ctrl_point_index
    ) -> const FbxVector2;
    
    auto ReadNormal
    (
        FbxMesh* fbx_mesh,
        const int& ctrl_point_index,
        const int& vertex_count
    ) -> const FbxVector4;
	
	auto FindBoneIndexFromName(const std::string &name) -> const uint;

private:
    class Context* context;
    std::string fbx_name;

    std::vector<struct FbxMaterialData> material_datas;
    std::vector<struct FbxMeshData> mesh_datas;
    std::vector<struct FbxBoneData> bone_datas;
    std::vector<struct FbxSkinnedAnimationData> animation_datas;

    std::unordered_map<uint, struct FbxCtrlPointData> ctrl_point_datas;
};
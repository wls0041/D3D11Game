#include "Framework.h"
#include "ModelImporter.h"
#include "FbxType.h"
#include "FbxUtility.h"

ModelImporter::ModelImporter(Context * context)
    : context(context)
    , fbx_name("")
{
}

ModelImporter::~ModelImporter()
{
}

void ModelImporter::Load(const std::string & path, Model** model)
{
    fbx_name = path; //TODO : FileSystem 만들고 처리

    //Create FbxManager
    auto manager = FbxManager::Create();
    assert(manager);

    //Create FbxScene
    auto scene = FbxScene::Create(manager, "");
    assert(scene);

    //Create FbxSettings
    auto ios = FbxIOSettings::Create(manager, IOSROOT);
    assert(ios);

    //IO Settings
    ios->SetBoolProp(IMP_FBX_TEXTURE, true);
    manager->SetIOSettings(ios);

    //Create Fbx Importer
    auto importer = FbxImporter::Create(manager, "");
    assert(importer);

    auto result = importer->Initialize(path.c_str(), -1, ios);
    assert(result);

    //Import FbxScene
    result = importer->Import(scene);
    assert(result);

    importer->Destroy();
    ios->Destroy();

    //Check Axis System
    auto scene_axis_system = scene->GetGlobalSettings().GetAxisSystem();
    if (scene_axis_system.GetCoorSystem() != FbxAxisSystem::DirectX.GetCoorSystem())
        FbxUtility::Axis_type = AxisType::Maya_Y_Up;
    else
        FbxUtility::Axis_type = AxisType::DirectX;

    //Check Unit System
    FbxSystemUnit scene_unit_system = scene->GetGlobalSettings().GetSystemUnit();
    if (scene_unit_system != FbxSystemUnit::m)
    {
        const FbxSystemUnit::ConversionOptions conversion_options = 
        {
            false,  // Rrs Node                 : 부모의 눈금을 상속받지 않는 노드도 변환 할것인가
            true,   // All Limits               : 제한선을 변환 할것인가
            true,   // Cluster                  : 클러스터(관절)을 변환 할것인가 
            true,   // Light Intensity          : 광도 속성을 변환 할것인가
            true,   // Photometic Properties    : 측광 라이트 속성을 변환 할것인가
            true    // Camera Clip Plane        : 카메라 클립평면을 변환 할것인가
        };

        FbxSystemUnit::m.ConvertScene(scene, conversion_options);
    }

    //Triangulater FbxScene
    FbxGeometryConverter geometry_converter(manager);
    geometry_converter.Triangulate(scene, true);

    //Process Model
    {
        ProcessMaterial(scene);
        //ProcessSkeletonHierarchy(scene);
        ProcessGeometryRecursively(scene->GetRootNode());
    }

    scene->Destroy();
    manager->Destroy();

    *model = new Model(context);
    for (const auto& mesh_data : mesh_datas)
        (*model)->AddGeometry(mesh_data);

    for (const auto& material_data : material_datas)
        (*model)->AddMaterial(material_data);
}

void ModelImporter::ExportFile()
{
    std::cout << "CtrlPoint Count : " << ctrl_point_datas.size() << std::endl;

    std::cout << std::endl;

    for (const auto& mesh_data : mesh_datas)
    {
        std::cout << "Mesh Name         : " << mesh_data.name.c_str() << std::endl;
        std::cout << "Mesh Vertex Count : " << mesh_data.vertices.size() << std::endl;
        std::cout << "Mesh Index Count  : " << mesh_data.indices.size() << std::endl;

        std::cout << std::endl;
    }

    std::cout << std::endl;

    std::cout << "Bone Count : " << bone_datas.size() << std::endl;

    std::cout << std::endl;

    for (const auto& bone_data : bone_datas)
    {
        std::cout << "Bone Name         : " << bone_data.name.c_str() << std::endl;
        std::cout << "Bone parent index : " << bone_data.parent_index << std::endl;
    }

    std::cout << std::endl;

    for (const auto& material_data : material_datas)
    {
        std::cout << "roughness : " << material_data.roughness << std::endl;
        std::cout << "metallic : " << material_data.metallic << std::endl;
        std::cout << "normal  : " << material_data.normal << std::endl;
        std::cout << "height : " << material_data.height << std::endl;
        std::cout << "albedo_texture_path : " << material_data.albedo_texture_path.c_str() << std::endl;
        std::cout << "roughness_texture_path : " << material_data.roughness_texture_path.c_str() << std::endl;
        std::cout << "metallic_texture_path : " << material_data.metallic_texture_path.c_str() << std::endl;
        std::cout << "normal_texture_path : " << material_data.normal_texture_path.c_str() << std::endl;
        std::cout << "height_texture_path : " << material_data.height_texture_path.c_str() << std::endl;
        std::cout << "occlusion_texture_path : " << material_data.occlusion_texture_path.c_str() << std::endl;
        std::cout << "emissive_texture_path : " << material_data.emissive_texture_path.c_str() << std::endl;
        std::cout << "mask_texture_path : " << material_data.mask_texture_path.c_str() << std::endl;

        std::cout << std::endl;
    }

}

void ModelImporter::ProcessMaterial(FbxScene * scene)
{
    for (int i = 0; i < scene->GetMaterialCount(); i++)
    {
        FbxMaterialData material_data;

        FbxSurfaceMaterial* surface_material = scene->GetMaterial(i);
        if (surface_material->GetClassId().Is(FbxSurfacePhong::ClassId))
        {
            FbxSurfacePhong* phong = static_cast<FbxSurfacePhong*>(surface_material);

            material_data.roughness = FbxUtility::ToFloat(phong->Shininess);
            material_data.metallic  = FbxUtility::ToFloat(phong->AmbientFactor);
            material_data.normal    = FbxUtility::ToFloat(phong->BumpFactor);
            material_data.height    = FbxUtility::ToFloat(phong->DisplacementFactor);
        }
        else if (surface_material->GetClassId().Is(FbxSurfaceLambert::ClassId))
        {
            FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(surface_material);

            material_data.metallic  = FbxUtility::ToFloat(lambert->AmbientFactor);
            material_data.normal    = FbxUtility::ToFloat(lambert->BumpFactor);
            material_data.height    = FbxUtility::ToFloat(lambert->DisplacementFactor);
        }

        const auto read_texture = [](FbxSurfaceMaterial* surface, const char* texture_type) -> const std::string
        {
            FbxProperty prop = surface->FindProperty(texture_type);

            if (prop.IsValid())
            {
                if (prop.GetSrcObjectCount() > 0)
                {
                    FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>();

                    if (texture)
                        return texture->GetFileName();
                }
            }

            return "N/A";
        };

        material_data.albedo_texture_path       = read_texture(surface_material, FbxSurfaceMaterial::sDiffuse);
        material_data.roughness_texture_path    = read_texture(surface_material, FbxSurfaceMaterial::sShininess);
        material_data.metallic_texture_path     = read_texture(surface_material, FbxSurfaceMaterial::sAmbient);
        material_data.normal_texture_path       = read_texture(surface_material, FbxSurfaceMaterial::sNormalMap);
        material_data.height_texture_path       = read_texture(surface_material, FbxSurfaceMaterial::sDisplacementColor);
        material_data.occlusion_texture_path    = read_texture(surface_material, FbxSurfaceMaterial::sSpecular);
        material_data.emissive_texture_path     = read_texture(surface_material, FbxSurfaceMaterial::sEmissive);
        material_data.mask_texture_path         = read_texture(surface_material, FbxSurfaceMaterial::sTransparentColor);

        material_datas.emplace_back(material_data);
    }
}

void ModelImporter::ProcessSkeletonHierarchy(FbxScene * scene)
{
    FbxNode* node = scene->GetRootNode();

    for (int i = 0; i < node->GetChildCount(); i++)
        ProcessSkeletonHierarchyRecursively(node->GetChild(i), 0, 0, -1);
}

void ModelImporter::ProcessSkeletonHierarchyRecursively(FbxNode * node, const int & depth, const int & index, const int & parent_index)
{
    auto attribute = node->GetNodeAttribute();

    if (attribute)
    {
        if (attribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
        {
            FbxBoneData bone_data;
            bone_data.name = std::string(node->GetName());
            bone_data.parent_index = parent_index;

            bone_datas.emplace_back(bone_data);
        }
    }

    for (int i = 0; i < node->GetChildCount(); i++)
        ProcessSkeletonHierarchyRecursively(node->GetChild(i), depth + 1, bone_datas.size(), index);
}

void ModelImporter::ProcessGeometryRecursively(FbxNode * node)
{
    auto attribute = node->GetNodeAttribute();

    if (attribute)
    {
        switch (attribute->GetAttributeType())
        {
        case FbxNodeAttribute::eMesh:
            ProcessCtrlPoints(node);
            //ProcessBone(node);
            ProcessMesh(node);
        }
    }

    for (int i = 0; i < node->GetChildCount(); i++)
        ProcessGeometryRecursively(node->GetChild(i));
}

void ModelImporter::ProcessCtrlPoints(FbxNode * node)
{
    auto fbx_mesh = node->GetMesh();

    if (!fbx_mesh)
        return;

    for (int i = 0; i < fbx_mesh->GetControlPointsCount(); i++)
    {
        FbxCtrlPointData ctrl_point_data;
        ctrl_point_data.position = fbx_mesh->GetControlPointAt(i);
        ctrl_point_datas[i] = ctrl_point_data;
    }
}

void ModelImporter::ProcessMesh(FbxNode * node)
{
    auto fbx_mesh = node->GetMesh();

    if (!fbx_mesh)
        return;

    FbxMeshData mesh_data;
    mesh_data.name = std::string(node->GetName());

    uint vertex_count = 0;
    for (int pi = 0; pi < fbx_mesh->GetPolygonCount(); pi++)
    {
        for (int i = 0; i < 3; i++)
        {
            int pvi = i;
            int ctrl_point_index = fbx_mesh->GetPolygonVertex(pi, pvi);

            VertexModel vertex;

            //Position
            vertex.position = FbxUtility::ToVector3(ctrl_point_datas[ctrl_point_index].position);

            //Uv
            vertex.uv = FbxUtility::ToUV(ReadUV(fbx_mesh, 0, pi, pvi, ctrl_point_index));

            //Normal
            vertex.normal = FbxUtility::ToVector3(ReadNormal(fbx_mesh, ctrl_point_index, vertex_count));

            //Indices

            //Weights

            mesh_data.vertices.emplace_back(vertex);
            mesh_data.indices.emplace_back(vertex_count);
            vertex_count++;
        }
    }

    mesh_datas.emplace_back(mesh_data);
}

auto ModelImporter::ReadUV(FbxMesh * fbx_mesh, const int & layer_index, const int & polygon_index, const int & polygon_vertex_index, const int & ctrl_point_index) -> const FbxVector2
{
    if (layer_index >= fbx_mesh->GetLayerCount())
        throw std::exception();

    auto layer = fbx_mesh->GetLayer(layer_index);
    if (!layer)
        throw std::exception();

    FbxLayerElementUV* uv = layer->GetUVs();

    int mapping_index = 0;
    switch (uv->GetMappingMode())
    {
    case FbxLayerElement::eByControlPoint:
    {
        switch (uv->GetReferenceMode())
        {
        case FbxLayerElement::eDirect:
            return uv->GetDirectArray().GetAt(ctrl_point_index);

        case FbxLayerElement::eIndexToDirect:
            mapping_index = uv->GetIndexArray().GetAt(ctrl_point_index);
            return uv->GetDirectArray().GetAt(mapping_index);
        }
        break;
    }

    case FbxLayerElement::eByPolygonVertex:
    {
        switch (uv->GetReferenceMode())
        {
        case FbxLayerElement::eDirect:
        case FbxLayerElement::eIndexToDirect:
            mapping_index = fbx_mesh->GetTextureUVIndex(polygon_index, polygon_vertex_index);
            return uv->GetDirectArray().GetAt(mapping_index);
        }
        break;
    }
    }

    throw std::exception();
}

auto ModelImporter::ReadNormal(FbxMesh * fbx_mesh, const int & ctrl_point_index, const int & vertex_count) -> const FbxVector4
{
    if (fbx_mesh->GetElementNormalCount() < 1)
        throw std::exception();

    FbxGeometryElementNormal* normal = fbx_mesh->GetElementNormal();

    int mapping_index = 0;
    switch (normal->GetMappingMode())
    {
    case FbxLayerElement::eByControlPoint:
    {
        switch (normal->GetReferenceMode())
        {
        case FbxLayerElement::eDirect:
            return normal->GetDirectArray().GetAt(ctrl_point_index);
        case FbxLayerElement::eIndexToDirect:
            mapping_index = normal->GetIndexArray().GetAt(ctrl_point_index);
            return normal->GetDirectArray().GetAt(mapping_index);
        }
        break;
    }

    case FbxLayerElement::eByPolygonVertex:
    {
        switch (normal->GetReferenceMode())
        {
        case FbxLayerElement::eDirect:
            return normal->GetDirectArray().GetAt(vertex_count);
        case FbxLayerElement::eIndexToDirect:
            mapping_index = normal->GetIndexArray().GetAt(vertex_count);
            return normal->GetDirectArray().GetAt(mapping_index);
        }
        break;
    }
    }

    throw std::exception();
}

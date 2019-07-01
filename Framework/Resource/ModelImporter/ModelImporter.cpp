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

void ModelImporter::Load(const std::string & path)
{
    fbx_name = path; //TODO : FileSystem ����� ó��

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

	//Create Fbx Importer -> fbx�����͸� ������
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
    if (scene_axis_system.GetCoorSystem() != FbxAxisSystem::DirectX.GetCoorSystem()) //������ ��ǥ�� directx�� ��ǥ�谡 �´°�
        FbxUtility::Axis_type = AxisType::Maya_Y_Up;
    else
        FbxUtility::Axis_type = AxisType::DirectX;

    //Check Unit System

    //Triangulater FbxScene
    FbxGeometryConverter geometry_converter(manager);
    geometry_converter.Triangulate(scene, true);


}

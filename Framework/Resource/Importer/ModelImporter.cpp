#include "Framework.h"
#include "ModelImporter.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/version.h>
#include "AssimpHelper.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"

ModelImporter::ModelImporter(Context * context) : context(context), model_path(NOT_ASSIGNED_STR), assimp_flags(0)
{
	scene_manager = context->GetSubsystem<SceneManager>();

	const int major = aiGetVersionMajor();
	const int minor = aiGetVersionMinor();
	const int revision = aiGetVersionRevision();

	assimp_flags = 
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices | //�ߺ� ���� ����
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality | //ĳ�� ������ ���. ����� ������ �ִ� �����͸� ���ڰ� ����
		aiProcess_LimitBoneWeights | //�� ����ġ ����
		aiProcess_SplitLargeMeshes | //�Ŵ��� �޽ø� ���ҽ��Ѽ� �������� �Ѱ� ���� ���� �ذ�
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType | //�������� ����
		aiProcess_FindDegenerates | //�ʿ���� ������ ����
		aiProcess_FindInvalidData |
		aiProcess_FindInstances | //�̹� �ν��Ͻ�ȭ �� �����ʹ� ���� ������ �ʰ� ������ ��
		aiProcess_ValidateDataStructure | //��ȿ���˻�
		aiProcess_Debone |
		aiProcess_ConvertToLeftHanded;
}

ModelImporter::~ModelImporter()
{
}

auto ModelImporter::Load(Model * model, const std::string & path) -> const bool
{
	model_path = path;

	Assimp::Importer importer;
	//����
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000); //�̰� �Ѿ�� �ɰ��� �� ��° �޽��� �ѱ�
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE); //��, �� ���������� �� ���� 
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

	//����
	importer.SetProgressHandler(new AssimpProgress(path));
	Assimp::DefaultLogger::set(new AssimpLogger());

	const auto scene = importer.ReadFile(model_path, assimp_flags);
	const auto result = scene != nullptr;

	if (result) {

	}
	else LOG_ERROR_F("%s", importer.GetErrorString()); //�� ���� ���ߴ��� ���� ���

	importer.FreeScene();
	return result;
}

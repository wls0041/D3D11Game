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
		aiProcess_JoinIdenticalVertices | //중복 정점 제거
		aiProcess_OptimizeMeshes |
		aiProcess_ImproveCacheLocality | //캐시 지역성 향상. 비슷한 지역에 있는 데이터를 예쁘게 정리
		aiProcess_LimitBoneWeights | //본 가중치 제한
		aiProcess_SplitLargeMeshes | //거대한 메시를 분할시켜서 정점개수 한계 등의 문제 해결
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_SortByPType | //연결정보 정렬
		aiProcess_FindDegenerates | //필요없는 정점을 삭제
		aiProcess_FindInvalidData |
		aiProcess_FindInstances | //이미 인스턴스화 된 데이터는 새로 만들지 않고 가져다 씀
		aiProcess_ValidateDataStructure | //유효성검사
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
	//설정
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyFloat(AI_CONFIG_PP_CT_MAX_SMOOTHING_ANGLE, 80.0f);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, 1'000'000);
	importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, 1'000'000); //이걸 넘어가면 쪼개서 두 번째 메쉬로 넘김
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE); //점, 선 빠져나오는 놈 삭제 
	importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
	importer.SetPropertyBool(AI_CONFIG_GLOB_MEASURE_TIME, true);

	//연결
	importer.SetProgressHandler(new AssimpProgress(path));
	Assimp::DefaultLogger::set(new AssimpLogger());

	const auto scene = importer.ReadFile(model_path, assimp_flags);
	const auto result = scene != nullptr;

	if (result) {

	}
	else LOG_ERROR_F("%s", importer.GetErrorString()); //왜 열지 못했는지 이유 출력

	importer.FreeScene();
	return result;
}

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

	if (result) ReadNodeHierarchy(scene, scene->mRootNode, model);
	else LOG_ERROR_F("%s", importer.GetErrorString()); //�� ���� ���ߴ��� ���� ���

	importer.FreeScene();
	return result;
}

void ModelImporter::ReadNodeHierarchy(const aiScene * assimp_scene, aiNode * assimp_node, Model * model, Actor * parent_actor, Actor * new_actor)
{
}

void ModelImporter::ReadAnimations(const aiScene * assimp_scene, Model * model)
{
}

void ModelImporter::LoadMesh(const aiScene * assimp_scene, aiMesh * assimp_mesh, Model * model, Actor * actor)
{
	if (!model || !assimp_scene || !assimp_mesh || !actor) {
		LOG_ERROR("Invalid Parameters");
		return;
	}

	//Vertices
	std::vector<VertexModel> vertices;
	{
		const auto vertex_count = assimp_mesh->mNumVertices;
		vertices.reserve(vertex_count);
		vertices.resize(vertex_count);

		for (uint i = 0; i < vertex_count; i++) {
			auto &vertex = vertices[i];

			if (assimp_mesh->mVertices)
				vertex.position = AssimpHelper::ToVector3(assimp_mesh->mVertices[i]);
		
			if (assimp_mesh->mNormals)
				vertex.normal = AssimpHelper::ToVector3(assimp_mesh->mNormals[i]);

			if (assimp_mesh->mTangents)
				vertex.tangent = AssimpHelper::ToVector3(assimp_mesh->mTangents[i]);

			const uint uv_channel = 0; //layer�� ���� ����
			if (assimp_mesh->HasTextureCoords(uv_channel)) 
				vertex.uv = AssimpHelper::ToVector2(assimp_mesh->mTextureCoords[uv_channel][i]);
		}
	}
	
	//Indices
	std::vector<uint> indices;
	{
		const auto index_count = assimp_mesh->mNumFaces * 3; //�ﰢ�� ���� * 3
		indices.reserve(index_count);
		indices.resize(index_count);

		for (uint i = 0; i < assimp_mesh->mNumFaces; i++) {
			auto &face = assimp_mesh->mFaces[i];
			indices[i * 3 + 0] = face.mIndices[0];
			indices[i * 3 + 1] = face.mIndices[1];
			indices[i * 3 + 2] = face.mIndices[2];
		}
	}

	//TODO : Renderable ����� ó��
	//TODO : Nodel ����� ó��

	if (assimp_scene->HasMaterials()) {
		const auto assimp_material = assimp_scene->mMaterials[assimp_mesh->mMaterialIndex];
		//TODO : model ����� ó��
	}
}

auto ModelImporter::LoadMaterial(const aiScene * assimp_scene, aiMaterial * assimp_material, Model * model) -> std::shared_ptr<Material>
{
	if (!model || !assimp_material) {
		LOG_ERROR("Invalid Parameters");
		return nullptr;
	}
	
	auto material = std::make_shared<Material>(context);

	//Name
	aiString material_name;
	aiGetMaterialString(assimp_material, AI_MATKEY_NAME, &material_name);
	material->SetResourceName(material_name.C_Str());

	//CULL_MODE
	int is_two_sided = 0;
	uint max = 1;

	if (AI_SUCCESS == aiGetMaterialIntegerArray(assimp_material, AI_MATKEY_TWOSIDED, &is_two_sided, &max));
	{
		if (is_two_sided != 0) material->SetCullMode(D3D11_CULL_NONE);
	}

	//Diffuse Color
	aiColor4D diffuse_color(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(assimp_material, AI_MATKEY_COLOR_DIFFUSE, &diffuse_color);

	//opacity
	aiColor4D opacity(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(assimp_material, AI_MATKEY_OPACITY, &opacity);


	material->SetAlbedoColor(Color4(diffuse_color.r, diffuse_color.g, diffuse_color.b, opacity.r));

	const auto LoadTexture = [&model, &assimp_scene, &assimp_material, &material, this](const aiTextureType &assimp_type, const TextureType &texture_type) {

	};
	/////////////////////���
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Albedo);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Roughness);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Metalic);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Normal);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Height);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Occlusion);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Emissive);
	LoadTexture(aiTextureType_DIFFUSE, TextureType::Mask);

	return nullptr;
}

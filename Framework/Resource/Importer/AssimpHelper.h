#pragma once
#include <assimp/scene.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>
#include "../ProgressReport.h"


class AssimpHelper final
{
public:
	static auto ToVector2(const aiVector2D &value) -> const Vector2;
	static auto ToVector2(const aiVector3D &value) -> const Vector2;
	static auto ToVector3(const aiVector3D &value) -> const Vector3;
	static auto ToColor(const aiColor4D &value) -> const Color4;
	static auto ToQuaternion(const aiQuaternion &value) -> const Quaternion;
	static auto ToMatrix(const aiMatrix4x4 &value) -> const Matrix;

	static void ComputeNodeCount(aiNode *node, int *count); //노드 개수 얻어옴
	static void ComputeActorTransform(aiNode *node, class Actor *actor); //노드에서 행렬빼서 액터에 집어넣음

	static auto Try_Multi_Extension() -> const std::string; //경로에 대응되는 확장자가 있는지 여러개를 대입해보고 확인
	static auto ValidatePath(const std::string &original_path, const std::string &model_path) -> const std::string;
};

class AssimpLogger : public Assimp::Logger
{
public:
	// Logger을(를) 통해 상속됨
	virtual bool attachStream(Assimp::LogStream * pStream, unsigned int severity = Debugging | Err | Warn | Info) override { return true; }
	virtual bool detatchStream(Assimp::LogStream * pStream, unsigned int severity = Debugging | Err | Warn | Info) override { return true; }

private:
	void OnDebug(const char * message) override {
#ifdef _DEBUG
		Log::caller_name = "ModelImporter::";
		Log::Write(message, LogType::Info);

#endif 
	}
	void OnInfo(const char * message) override {
		Log::caller_name = "ModelImporter::";
		Log::Write(message, LogType::Info);
	}
	void OnWarn(const char * message) override {
		Log::caller_name = "ModelImporter::";
		Log::Write(message, LogType::Warning);
	}
	void OnError(const char * message) override {
		Log::caller_name = "ModelImporter::";
		Log::Write(message, LogType::Error);
	}
};

class AssimpProgress final : public Assimp::ProgressHandler
{
public:
	AssimpProgress(const std::string &path) : path(path), name(FileSystem::GetFileNameFromPath(path)) {
		auto &progress = ProgressReport::Get();
		progress.Reset(ProgressReport::Model);
		progress.SetIsLoading(ProgressReport::Model, true);
	}
	~AssimpProgress() {

	}
	// ProgressHandler을(를) 통해 상속됨
	bool Update(float percentage = (-1.0F)) override { return true; }

	void UpdateFileRead(int current_step, int number_of_step) override {
		auto &progress = ProgressReport::Get();
		progress.SetStatus(ProgressReport::Model, "Loading... \"" + name + "\""); //후처리 말하는거 아님
		progress.SetJobsDone(ProgressReport::Model, current_step);
		progress.SetJobCount(ProgressReport::Model, number_of_step);
	}

	void UpdatePostProcess(int current_step, int number_of_step) override {
		auto &progress = ProgressReport::Get();
		progress.SetStatus(ProgressReport::Model, "Post Processing... \"" + name + "\"");
		progress.SetJobsDone(ProgressReport::Model, current_step);
		progress.SetJobCount(ProgressReport::Model, number_of_step);
	}

private:
	std::string path;
	std::string name;
};
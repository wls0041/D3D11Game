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

	static void ComputeNodeCount(aiNode *node, int *count); //��� ���� ����
	static void ComputeActorTransform(aiNode *node, class Actor *actor); //��忡�� ��Ļ��� ���Ϳ� �������

	static auto Try_Multi_Extension() -> const std::string; //��ο� �����Ǵ� Ȯ���ڰ� �ִ��� �������� �����غ��� Ȯ��
	static auto ValidatePath(const std::string &original_path, const std::string &model_path) -> const std::string;
};

class AssimpLogger : public Assimp::Logger
{
public:
	// Logger��(��) ���� ��ӵ�
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
	// ProgressHandler��(��) ���� ��ӵ�
	bool Update(float percentage = (-1.0F)) override { return true; }

	void UpdateFileRead(int current_step, int number_of_step) override {
		auto &progress = ProgressReport::Get();
		progress.SetStatus(ProgressReport::Model, "Loading... \"" + name + "\""); //��ó�� ���ϴ°� �ƴ�
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
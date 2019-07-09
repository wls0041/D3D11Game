#pragma once

struct Material {
	ID3D11ShaderResourceView *diffuse;
};

struct Mesh {
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
};

struct Bone {
	std::string name;
	int parent_index;
	D3DXMATRIX mat_bind_pose;
	D3DXMATRIX mat_offset;
};

struct Keyframe {
	float time;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 position;
	D3DXQUATERNION quaternion;
};

struct AnimationNode {
	auto GetStartTime() const -> const float;
	auto GetEndTime() const -> const float;

	void Interpolate(const float &t, D3DXMATRIX &mat) const;

	std::string name;
	std::vector<Keyframe> keyframes;
};

class Animation {
public:
	Animation() = default;
	~Animation() = default;

	auto GetName() const -> const std::string& { return name; }
	void SetName(const std::string& name) { this->name = name; }

	auto GetFrameCount() const -> const int& { return frame_count; }
	void SetFrameCount(const int& frame_count) { this->frame_count = frame_count; }

	auto GetFrameRate() const -> const float& { return frame_rate; }
	void SetFrameRate(const float& frame_rate) { this->frame_rate = frame_rate; }

	auto GetDuration() const -> const float& { return duration; }
	void SetDuration(const float& duration) { this->duration = duration; }

	auto GetChannels() const -> const std::vector<AnimationNode>& { return channels; }
	void SetChannels(const AnimationNode& channel);

	auto GetClipStartTime() const -> const float;
	auto GetClipEndTime() const -> const float;

	void Interpolate(const float &t, std::vector<D3DXMATRIX> &matrixs);

private:
	std::string name;
	int frame_count;
	float frame_rate;
	float duration;
	std::vector<AnimationNode> channels;
};

class Model final
{
public:
	Model(class Context *context);
	~Model();

	void AddGeometry(const struct FbxMeshData &mesh_data);
	void AddMaterial(const struct FbxMaterialData &material_data);
	void AddBone(const struct FbxBoneData &bone_data);
	void AddAnimation(const struct FbxSkinnedAnimationData &animation_data);

	void Update();
	void Render();

private:
	class Context *context;
	class Graphics *graphics;
	class Timer *timer;

	InputLayout* inputLayout;
	VertexShader* vertexShader;
	PixelShader* pixelShader;
	ConstantBuffer* worldBuffer;
	D3DXMATRIX world;

	float frame_timer;

	std::vector<Mesh> meshes;
	std::vector<Material> materials;

	std::vector<Bone> bones;
	std::vector<D3DXMATRIX> skinned_transforms;
	std::vector<Animation> animations;
};
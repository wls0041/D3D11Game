#include "Framework.h"
#include "Model.h"
#include "./ModelImporter/FbxType.h"

auto AnimationNode::GetStartTime() const -> const float
{
	return keyframes.front().time;
}

auto AnimationNode::GetEndTime() const -> const float
{
	return keyframes.back().time;
}

void AnimationNode::Interpolate(const float & t, D3DXMATRIX & mat) const
{
	//애니메이션 시작 시점
	if (t <= GetStartTime()) {
		auto s = keyframes.front().scale;
		auto r = keyframes.front().quaternion;
		auto p = keyframes.front().position;

		D3DXMATRIX S, R, T;
		D3DXMatrixScaling(&S, s.x, s.y, s.z);
		D3DXMatrixRotationQuaternion(&R, &r);
		D3DXMatrixTranslation(&T, p.x, p.y, p.z);

		mat = S * R * T;
	}
	//애니메이션 끝남
	else if (t >= GetEndTime()) {
		auto s = keyframes.back().scale;
		auto r = keyframes.back().quaternion;
		auto p = keyframes.back().position;

		D3DXMATRIX S, R, T;
		D3DXMatrixScaling(&S, s.x, s.y, s.z);
		D3DXMatrixRotationQuaternion(&R, &r);
		D3DXMatrixTranslation(&T, p.x, p.y, p.z);

		mat = S * R * T;
	}
	//진행 중
	else {
		for (uint i = 0; i < keyframes.size(); i++) {
			if (keyframes[i].time <= t && keyframes[i + 1].time >= t) {
				float f = (t - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time); //한 프레임 안에서 진행된 시간 / 한 프레임의 전체 시간 = 한 프레임의 진행정도
				
				auto s = D3DXVECTOR3(1, 1, 1);
				auto s0 = keyframes[i].scale;
				auto s1 = keyframes[i + 1].scale;

				auto r = D3DXQUATERNION(0 , 0, 0, 1);
				auto r0 = keyframes[i].quaternion;
				auto r1 = keyframes[i + 1].quaternion;

				auto p = D3DXVECTOR3(0, 0, 0);
				auto p0 = keyframes[i].position;
				auto p1 = keyframes[i + 1].position;
			
				//Linear Interpolate
				D3DXVec3Lerp(&s, &s0, &s1, f); //선형보관
				D3DXVec3Lerp(&p, &p0, &p1, f);
				D3DXQuaternionSlerp(&r, &r0, &r1, f); //Spherical Linear Interpolate. 구면보관

				D3DXMATRIX S, R, T;
				D3DXMatrixScaling(&S, s.x, s.y, s.z);
				D3DXMatrixRotationQuaternion(&R, &r);
				D3DXMatrixTranslation(&T, p.x, p.y, p.z);

				mat = S * R * T;
				break;
			}
		}
	}
}

void Animation::SetChannels(const AnimationNode & channel)
{
	channels.emplace_back(channel);
}

auto Animation::GetClipStartTime() const -> const float
{
	float t = std::numeric_limits<float>::max();
	for (const auto &channel : channels) t = std::min(t, channel.GetStartTime());

	return t;
}

auto Animation::GetClipEndTime() const -> const float
{
	float t = std::numeric_limits<float>::min();
	for (const auto &channel : channels) t = std::max(t, channel.GetEndTime());

	return t;
}

void Animation::Interpolate(const float & t, std::vector<D3DXMATRIX>& matrixs)
{
	int count = 0;
	for (const auto &channel : channels) {
		channel.Interpolate(t, matrixs[count]);
		count++;
	}
}

////////////////////////////////////////////////////////////////////////////////////

Model::Model(Context * context)
    : context(context)
	, frame_timer(0.0f)
{
    graphics = context->GetSubsystem<Graphics>();
	timer = context->GetSubsystem<Timer>();

    vertexShader = new VertexShader(context);
    vertexShader->Create("../../_Assets/Shader/Model.hlsl", "VS", "vs_5_0");

    pixelShader = new PixelShader(context);
    pixelShader->Create("../../_Assets/Shader/Model.hlsl", "PS", "ps_5_0");

    inputLayout = new InputLayout(context);
    inputLayout->Create(vertexShader->GetBlob());

    worldBuffer = new ConstantBuffer(context);
    worldBuffer->Create<WorldData>();

	skinnedBuffer = new ConstantBuffer(context);
	skinnedBuffer->Create<SkinnedData>();

    D3DXMatrixIdentity(&world);
}

Model::~Model()
{
    for (auto& mesh : meshes)
    {
        SAFE_DELETE(mesh.indexBuffer);
        SAFE_DELETE(mesh.vertexBuffer);
    }

    SAFE_DELETE(inputLayout);
    SAFE_DELETE(vertexShader);
    SAFE_DELETE(pixelShader);
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(skinnedBuffer);
}

void Model::AddGeometry(const FbxMeshData & mesh_data)
{
    Mesh mesh;
    mesh.vertexBuffer = new VertexBuffer(context);
    mesh.vertexBuffer->Clear();
    mesh.vertexBuffer->Create(mesh_data.vertices);

    mesh.indexBuffer = new IndexBuffer(context);
    mesh.indexBuffer->Clear();
    mesh.indexBuffer->Create(mesh_data.indices);

    meshes.emplace_back(mesh);
}

void Model::AddMaterial(const FbxMaterialData & material_data)
{
    Material material;
    auto hr = D3DX11CreateShaderResourceViewFromFileA
    (
        graphics->GetDevice(),
        material_data.albedo_texture_path.c_str(),
        nullptr,
        nullptr,
        &material.diffuse,
        nullptr
    );
    assert(SUCCEEDED(hr));

    materials.emplace_back(material);
}

void Model::AddBone(const FbxBoneData & bone_data)
{
	Bone bone;
	bone.name = bone_data.name;
	bone.parent_index = bone_data.parent_index;
	bone.mat_bind_pose = bone_data.bind_pose_transform;
	bone.mat_offset = bone_data.offset_transform;

	bones.emplace_back(bone);
}

void Model::AddAnimation(const FbxSkinnedAnimationData & animation_data)
{
	Animation animation;
	animation.SetName(animation_data.name);
	animation.SetFrameCount(animation_data.frame_count);
	animation.SetFrameRate(animation_data.frame_rate);
	animation.SetDuration(animation_data.duration);

	for (const auto &bone_animation : animation_data.bone_animations) {
		AnimationNode animation_node;
		animation_node.name = bone_animation.name;

		for (const auto &keyframe : bone_animation.keyframes) {
			D3DXVECTOR3 s, p;
			D3DXQUATERNION q;

			D3DXMatrixDecompose(&s, &q, &p, &keyframe.transform);

			Keyframe key;
			key.time = keyframe.time;
			key.scale = s;
			key.position = p;
			key.quaternion = q;
		
			animation_node.keyframes.emplace_back(key);
		}
		animation.SetChannels(animation_node);
	}

	animations.emplace_back(animation);
}

void Model::Update()
{
    auto world_data = worldBuffer->Map<WorldData>();
    {
        D3DXMatrixTranspose(&world_data->world, &world);
    }
    worldBuffer->Unmap();

	/////////////////////////////////////////////////////////
	uint bone_count = bones.size();

	if (skinned_transforms.empty()) {
		skinned_transforms.reserve(bone_count);
		skinned_transforms.resize(bone_count);
	}

	frame_timer += timer->GetDeltaTimeMs();
	if (frame_timer > animations[1].GetClipEndTime()) frame_timer = 0; //원래는 1로 지정해서 하면 안됨

	//보간된 애니메이션 행렬
	std::vector<D3DXMATRIX> animation_transforms(bone_count);
	animations[1].Interpolate(frame_timer, animation_transforms);

	//보간된 애니메이션 행렬을 월드 공간으로 이동
	std::vector<D3DXMATRIX> bone_animation_transforms(bone_count);
	for (uint i = 0; i < bone_count; i++) {
		if (i == 0) bone_animation_transforms[0] = animation_transforms[0]; //Root Bone
		else {
			auto parent_index = bones[i].parent_index; 
			auto mat_animation = animation_transforms[i];
			auto mat_parent_animation = bone_animation_transforms[parent_index];

			bone_animation_transforms[i] = mat_animation * mat_parent_animation; //이미 bone_animation에 부모가 계산됨. 따라서 곱해주면 끝. 이를 반복
		}

		auto mat_bone = bones[i].mat_offset;
		skinned_transforms[i] = mat_bone * bone_animation_transforms[i]; //skin에 bone을 입힘
	}

	auto skinned_data = skinnedBuffer->Map<SkinnedData>();
	{
		for (uint i = 0; i < bone_count; i++) D3DXMatrixTranspose(&skinned_data->skinned_transforms[i], &skinned_transforms[i]);
	}
	skinnedBuffer->Unmap();
}

void Model::Render()
{
    inputLayout->BindPipeline();
    vertexShader->BindPipeline();
    pixelShader->BindPipeline();
	worldBuffer->BindPipeline(1, ShaderScope::VS);
	skinnedBuffer->BindPipeline(2, ShaderScope::VS);

    graphics->GetDeviceContext()->PSSetShaderResources(0, 1, &materials[0].diffuse);
    graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto& mesh : meshes)
    {
        mesh.vertexBuffer->BindPipeline();
        mesh.indexBuffer->BindPipeline();
        graphics->GetDeviceContext()->DrawIndexed(mesh.indexBuffer->GetCount(), 0, 0);
    }
}

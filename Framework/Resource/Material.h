#pragma once
#include "IResource.h"

enum class TextureType : uint
{
	Unknown,
	Albedo, //본인이 발산하는 고유색 (디퓨즈와 다른 점은 음영을 포함하지 않는다는 것)
	Roughness, //거칠기
	Metallic, //ROUGHTNESS와 METALIC은 두 정점 사이에 미세한 굴곡이 있다고 가정해서 더 현신감 있는 표면을 만들어줌
	Normal, //표면질감. 굴곡이 있는 것처럼 만들어줌.
	Height, //노말맵만 사용시 실제 굴곡이 아님으로 인한 오류를 해결해줌
	Occlusion, //가려지는 부분 색 처리. 그림자
	Emissive,  //방출되는 빛
	Mask,    //특정 부위를 masking. 원하는 부위만 색 바꿀 수 있음
};

enum class ShadingMode : uint
{
	SKY,
	PBR,
};

class Material final : public IResource
{
public:
	Material(class Context *context);
	~Material();

	const bool SaveToFile(const std::string &path) override;
	const bool LoadFromFile(const std::string &path) override;

	auto GetShader() -> const std::shared_ptr<class Shader>& { return shader; }
	void SetShader(const std::shared_ptr<class Shader> &shader) { this->shader = shader; }
	void SetStandardShader();

	auto GetTexture(const TextureType& type)->std::shared_ptr<Texture>; //texture가 없을 수 있으므로 const 참조 x
	auto GetTextureShaderResource(const TextureType& type)->ID3D11ShaderResourceView*;
	auto GetTextureShaderResources()->std::vector<ID3D11ShaderResourceView*>;

	void SetTexture(const TextureType& type, const std::shared_ptr<Texture> &texture);
	void SetTexture(const TextureType& type, const std::string& path);

	auto HasTexture(const TextureType& type)->const bool;
	auto HasTexture(const std::string& path)->const bool;

	auto GetShadingMode() const -> const ShadingMode& { return shading_mode; }
	void SetShadingMode(const ShadingMode& shading_mode) { this->shading_mode = shading_mode; }

	auto GetCullMode() const -> const D3D11_CULL_MODE& { return cull_mode; }
	void SetCullMode(const D3D11_CULL_MODE& cull_mode) { this->cull_mode = cull_mode; }

	auto GetRoughnessCoefficient() const -> const float& { return roughness_coef; }
	void SetRoughnessCoefficient(const float& roughness_coef) { this->roughness_coef = roughness_coef; }

	auto GetMetallicCoefficient() const -> const float& { return metallic_coef; }
	void SetMetallicCoefficient(const float& metallic_coef) { this->metallic_coef = metallic_coef; }

	auto GetNormalCoefficient() const -> const float& { return normal_coef; }
	void SetNormalCoefficient(const float& normal_coef) { this->normal_coef = normal_coef; }

	auto GetHeightCoefficient() const ->const float& { return height_coef; }
	void SetHeightCoefficient(const float& height_coef) { this->height_coef = height_coef; }

	auto GetAlbedoColor() const ->const Color4& { return albedo_color; }
	void SetAlbedoColor(const Color4& aledo_color) { this->albedo_color = aledo_color; }

	auto GetTiling() const ->const Vector2& { return uv_tiling; }
	void SetTiling(const Vector2& uv_tiling) { this->uv_tiling = uv_tiling; }

	auto GetOffset() const ->const Vector2& { return uv_offset; }
	void SetOffset(const Vector2& uv_offset) { this->uv_offset = uv_offset; }

	///////////////Constant Buffer//////////////////////
	void UpdateConstantBuffer();
	auto GetConstantBuffer() const -> const std::shared_ptr<class ConstantBuffer>& { return gpu_buffer; }

private:
	std::shared_ptr<class Shader> shader;

	ShadingMode shading_mode;
	D3D11_CULL_MODE cull_mode;

	Color4 albedo_color;
	float roughness_coef; //coefficient -> 게수
	float metallic_coef;
	float normal_coef;
	float height_coef;

	Vector2 uv_tiling;
	Vector2 uv_offset;

	std::map<TextureType, std::shared_ptr<Texture>> textures;

	///////////////Constant Buffer//////////////////////
	MATERIAL_DATA cpu_buffer;
	std::shared_ptr<class ConstantBuffer> gpu_buffer;
};
#include "Framework.h"
#include "Material.h"
#include "../Shader/Standard_Shader.h"

Material::Material(Context * context)
	: IResource(context, ResourceType::Material)
	, shading_mode(ShadingMode::PBR)
	, cull_mode(D3D11_CULL_BACK)
	, albedo_color(1.0f)
	, roughness_coef(1.0f)
	, metalic_coef(0.0f)
	, normal_coef(0.0f)
	, height_coef(0.0f)
	, uv_tiling(1.0f)
	, uv_offset(0.0f)
{
}

Material::~Material()
{
}

const bool Material::SaveToFile(const std::string & path)
{
	return false;
}

const bool Material::LoadFromFile(const std::string & path)
{
	return false;
}

void Material::SetStandardShader()
{
	uint shader_flags = 0U;

	if (HasTexture(TextureType::Albedo))    shader_flags |= Shader_Flags_Albedo;
	if (HasTexture(TextureType::Roughness)) shader_flags |= Shader_Flags_Roughness;
	if (HasTexture(TextureType::Metallic))  shader_flags |= Shader_Flags_Metallic;
	if (HasTexture(TextureType::Normal))    shader_flags |= Shader_Flags_Normal;
	if (HasTexture(TextureType::Height))    shader_flags |= Shader_Flags_Height;
	if (HasTexture(TextureType::Occlusion)) shader_flags |= Shader_Flags_Occlusion;
	if (HasTexture(TextureType::Emissive))  shader_flags |= Shader_Flags_Emissive;
	if (HasTexture(TextureType::Mask))      shader_flags |= Shader_Flags_Mask;

	shader = Standard_Shader::GetMatchingStandardShader(context, shader_flags);
}

auto Material::GetTexture(const TextureType & type) -> std::shared_ptr<Texture>
{
	return HasTexture(type) ? textures[type] : nullptr;
}

auto Material::GetTextureShaderResource(const TextureType & type) -> ID3D11ShaderResourceView *
{
	auto texture = GetTexture(type);

	return texture ? texture->GetShaderResourceView() : nullptr;
}

auto Material::GetTextureShaderResources() -> std::vector<ID3D11ShaderResourceView *>
{

	std::vector<ID3D11ShaderResourceView *> shader_resources
	{
		GetTextureShaderResource(TextureType::Albedo),
		GetTextureShaderResource(TextureType::Roughness),
		GetTextureShaderResource(TextureType::Metallic),
		GetTextureShaderResource(TextureType::Normal),
		GetTextureShaderResource(TextureType::Height),
		GetTextureShaderResource(TextureType::Occlusion),
		GetTextureShaderResource(TextureType::Emissive),
		GetTextureShaderResource(TextureType::Mask),
	};

	return shader_resources;
}

void Material::SetTexture(const TextureType & type, const std::shared_ptr<Texture> &texture)
{
	if (!texture)
	{
		LOG_ERROR("Invalid parameter. texture is nullptr");
		return;
	}

	textures[type] = texture;
	SetStandardShader();
}

void Material::SetTexture(const TextureType & type, const std::string & path)
{
	auto texture = context->GetSubsystem<ResourceManager>()->Load<Texture2D>(path);
	SetTexture(type, texture);
}

auto Material::HasTexture(const TextureType & type) -> const bool
{
	return textures.find(type) != textures.end();
}

auto Material::HasTexture(const std::string & path) -> const bool
{
	for (const auto& texture : textures)
		if (texture.second->GetResourcePath() == path) return true;

	return false;
}

void Material::UpdateConstantBuffer()
{

	if (!gpu_buffer)
	{
		gpu_buffer = std::make_shared<ConstantBuffer>(context);
		gpu_buffer->Create<TRANSFORM_DATA>();
	}

	auto is_update = false; //같으면 갱신할 필요 없으므로 체크
	is_update |= cpu_buffer.albedo_color	!= albedo_color ? true : false;
	is_update |= cpu_buffer.tiling			!= uv_tiling ? true : false;
	is_update |= cpu_buffer.offset			!= uv_offset ? true : false;
	is_update |= cpu_buffer.roughness_coef	!= roughness_coef ? true : false;
	is_update |= cpu_buffer.metallic_coef	!= metalic_coef ? true : false;
	is_update |= cpu_buffer.normal_coef		!= normal_coef ? true : false;
	is_update |= cpu_buffer.height_coef		!= height_coef ? true : false;
	is_update |= cpu_buffer.shading_mode	!= static_cast<float>(shading_mode) ? true : false;

	if (!is_update) return;

	auto gpu_data = gpu_buffer->Map<MATERIAL_DATA>();
	if (!gpu_data) {
		LOG_ERROR("Failed to map buffer");
		return;
	}

	gpu_data->albedo_color	= cpu_buffer.albedo_color = albedo_color; //gpu에 전달하기 위해 constantbuffer에 입력
	gpu_data->tiling		= cpu_buffer.tiling = uv_tiling;
	gpu_data->offset		= cpu_buffer.offset = uv_offset;
	gpu_data->roughness_coef= cpu_buffer.roughness_coef = roughness_coef;
	gpu_data->metallic_coef	= cpu_buffer.metallic_coef = metalic_coef;
	gpu_data->normal_coef	= cpu_buffer.normal_coef = normal_coef;
	gpu_data->height_coef	= cpu_buffer.height_coef = height_coef;
	gpu_data->shading_mode	= cpu_buffer.shading_mode	= static_cast<float>(shading_mode);

	gpu_buffer->Unmap();

}

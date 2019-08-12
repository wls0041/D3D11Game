#include "Framework.h"
#include "Material.h"
#include "../Shader/Standard_Shader.h"

Material::Material(Context * context)
	: IResource(context, ResourceType::Material)
	, shading_mode(ShadingMode::PBR)
	, cull_mode(D3D11_CULL_BACK)
	, aledo_color(1.0f)
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

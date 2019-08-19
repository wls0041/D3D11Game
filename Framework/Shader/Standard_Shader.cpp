#include "Framework.h"
#include "Standard_Shader.h"

std::map<Shader_Flags, std::shared_ptr<Shader>> Standard_Shader::standard_shaders;

auto Standard_Shader::GetMatchingStandardShader(Context * context, const uint & shader_flags) -> const std::shared_ptr<class Shader>
{
	auto pair_ib = standard_shaders.insert
	(
		std::make_pair(static_cast<Shader_Flags>(shader_flags), std::make_shared<Shader>(context))
	);

	if (pair_ib.second)
	{
        auto shader = pair_ib.first->second;

        shader->AddDefine("ALBEDO_TEXTURE",     shader_flags & Shader_Flags_Albedo      ? "1" : "0");
        shader->AddDefine("ROUGHNESS_TEXTURE",  shader_flags & Shader_Flags_Roughness   ? "1" : "0");
        shader->AddDefine("METALLIC_TEXTURE",   shader_flags & Shader_Flags_Metallic    ? "1" : "0");
        shader->AddDefine("NORMAL_TEXTURE",     shader_flags & Shader_Flags_Normal      ? "1" : "0");
        shader->AddDefine("HEIGHT_TEXTURE",     shader_flags & Shader_Flags_Height      ? "1" : "0");
        shader->AddDefine("OCCLUSION_TEXTURE",  shader_flags & Shader_Flags_Occlusion   ? "1" : "0");
        shader->AddDefine("EMISSION_TEXTURE",   shader_flags & Shader_Flags_Emissive    ? "1" : "0");
        shader->AddDefine("MASK_TEXTURE",       shader_flags & Shader_Flags_Mask        ? "1" : "0");

        auto directory = context->GetSubsystem<ResourceManager>()->GetAssetDirectory(AssetType::Shader);

        shader->AddShader<PixelShader>(directory + "GBuffer.hlsl");
	}

	return pair_ib.first->second;
}

#pragma once

enum Shader_Flags : uint
{
	Shader_Flags_Albedo		= 1U << 0,
	Shader_Flags_Roughness	= 1U << 1,
	Shader_Flags_Metallic	= 1U << 2,
	Shader_Flags_Normal		= 1U << 3,
	Shader_Flags_Height		= 1U << 4,
	Shader_Flags_Occlusion	= 1U << 5,
	Shader_Flags_Emissive	= 1U << 6,
	Shader_Flags_Mask		= 1U << 7,
};

class Standard_Shader final
{
public:
	static auto GetMatchingStandardShader
	(
		class Context* context,
		const uint& shader_flags
	) -> const std::shared_ptr<class Shader>;

private:
	static std::map<Shader_Flags, std::shared_ptr<Shader>> standard_shaders;
};
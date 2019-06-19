#pragma once

class IShaderStage
{
public:
	IShaderStage(class Context *context);

	virtual ~IShaderStage() = default;

	virtual auto ShaderErrorHandler(const HRESULT &hr, ID3DBlob *error) -> const bool;
	virtual auto CompileShader(const std::string path, const std::string &function_name, const std::string &profile, D3D_SHADER_MACRO *defines, ID3DBlob **blob) -> const bool;

	virtual void Create(const std::string path, const std::string &function_name, const std::string &profile, D3D_SHADER_MACRO *define = nullptr) = 0; //매크로함수 오버로딩 하지 말것
	virtual void Clear() = 0;
	virtual void BindPipeline() = 0;

protected:
	class Graphics *graphics;
	std::string path;
	std::string function_name;
	std::string profile;
};
#include "Framework.h"
#include "IShaderStage.h"

IShaderStage::IShaderStage(Context * context) : path(NOT_ASSIGNED_STR), function_name(NOT_ASSIGNED_STR), profile(NOT_ASSIGNED_STR)
{
	graphics = context->GetSubsystem<Graphics>();
}

auto IShaderStage::ShaderErrorHandler(const HRESULT & hr, ID3DBlob * error) -> const bool
{
	if (FAILED(hr))
	{
		if (error)
		{
			std::string str = reinterpret_cast<const char*>(error->GetBufferPointer());
			MessageBoxA(nullptr, str.c_str(), "Shader ERROR!!", MB_OK);
		}
		return false;
	}
	return false;
}

auto IShaderStage::CompileShader(const std::string path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines, ID3DBlob ** blob) -> const bool
{
#ifdef _DEBUG
	uint flags = D3DCOMPILE_ENABLE_STRICTNESS;

#else
	uint flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;

#endif

	ID3DBlob *error = nullptr;
	auto hr = D3DCompileFromFile(path.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, function_name.c_str(), profile.c_str(), flags, 0, blob, &error);

	auto result = ShaderErrorHandler(hr, error);
	SAFE_RELEASE(error);

	return result;
}

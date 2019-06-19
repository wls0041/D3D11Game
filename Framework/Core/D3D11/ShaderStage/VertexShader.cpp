#include "Framework.h"
#include "VertexShader.h"

VertexShader::VertexShader(class Context *context) : IShaderStage(context), shader(nullptr), blob(nullptr)
{
}

VertexShader::~VertexShader()
{
	Clear();
}

void VertexShader::Create(const std::string path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * define = nullptr)
{
}

void VertexShader::Clear()
{
	SAFE_RELEASE(shader);
	SAFE_RELEASE(blob);
	path = NOT_ASSIGNED_STR;
	function_name = NOT_ASSIGNED_STR;
	profile = NOT_ASSIGNED_STR;
}

void VertexShader::BindPipeline()
{
	graphics->GetDeviceContext()->;
}

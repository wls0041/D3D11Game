#include "Framework.h"
#include "PixelShader.h"

PixelShader::PixelShader(Context * context)
    : IShaderStage(context)
    , shader(nullptr)
    , blob(nullptr)
{
}

PixelShader::~PixelShader()
{
    Clear();
}

void PixelShader::Create(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines)
{
    this->path          = path;
    this->function_name = function_name != NOT_ASSIGNED_STR ? function_name : "PS";
    this->profile       = profile       != NOT_ASSIGNED_STR ? profile       : "ps_5_0";

    auto result = CompileShader
    (
        this->path,
        this->function_name,
        this->profile,
        defines,
        &blob
    );

    if (!result)
        return;

    auto hr = graphics->GetDevice()->CreatePixelShader
    (
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        nullptr,
        &shader
    );
    assert(SUCCEEDED(hr));
}

void PixelShader::Clear()
{
    SAFE_RELEASE(shader);
    SAFE_RELEASE(blob);
    path            = NOT_ASSIGNED_STR;
    function_name   = NOT_ASSIGNED_STR;
    profile         = NOT_ASSIGNED_STR;
}

void PixelShader::BindPipeline()
{
    graphics->GetDeviceContext()->PSSetShader(shader, nullptr, 0);
}

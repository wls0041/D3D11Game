#pragma once
#include "IShaderStage.h"

class PixelShader final : public IShaderStage
{
public:
    PixelShader(class Context* context);
    ~PixelShader();

    auto GetResource() const -> ID3D11PixelShader* { return shader; }
    auto GetBlob() const -> ID3DBlob* { return blob; }

    void Create(const std::string & path, const std::string & function_name, const std::string & profile, D3D_SHADER_MACRO * defines = nullptr) override;
    void Clear() override;
    void BindPipeline() override;

private:
    ID3D11PixelShader* shader;
    ID3DBlob* blob;
};
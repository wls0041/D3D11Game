#pragma once

class IShaderStage
{
public:
    IShaderStage(class Context* context);
    virtual ~IShaderStage() = default;

    virtual void Create
    (
        const std::string& path,
        const std::string& function_name,
        const std::string& profile,
        D3D_SHADER_MACRO* defines = nullptr
    ) = 0;
    virtual void Clear() = 0;
    virtual void BindPipeline() = 0;

protected:
    virtual auto ShaderErrorHandler(const HRESULT& hr, ID3DBlob* error) -> const bool;
    virtual auto CompileShader
    (
        const std::string& path,
        const std::string& function_name,
        const std::string& profile,
        D3D_SHADER_MACRO* defines,
        ID3DBlob** blob
    ) -> const bool;

protected:
    class Graphics* graphics;
    std::string path;
    std::string function_name;
    std::string profile;
};
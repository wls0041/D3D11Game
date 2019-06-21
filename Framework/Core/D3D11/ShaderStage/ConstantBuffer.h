#pragma once

class ConstantBuffer final
{
public:
    ConstantBuffer(class Context* context);
    ~ConstantBuffer();

    auto GetResource() const -> ID3D11Buffer* { return buffer; }

    void Create();
    void Clear();
    void BindPipeline();

private:
    class Graphics* graphics;
    ID3D11Buffer* buffer;
};
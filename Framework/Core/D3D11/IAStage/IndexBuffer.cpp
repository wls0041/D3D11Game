#include "Framework.h"
#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Context * context)
    : buffer(nullptr)
    , stride(0)
    , offset(0)
    , count(0)
{
    graphics = context->GetSubsystem<Graphics>();
}

IndexBuffer::~IndexBuffer()
{
    Clear();
}

void IndexBuffer::Create(const std::vector<uint>& indices, const D3D11_USAGE & usage)
{
    stride = sizeof(uint);
    count = indices.size();

    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

    switch (usage)
    {
    case D3D11_USAGE_IMMUTABLE:
    case D3D11_USAGE_DEFAULT:
        desc.CPUAccessFlags = 0;
        break;

    case D3D11_USAGE_DYNAMIC:
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        break;
    case D3D11_USAGE_STAGING:
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        break;
    }

    desc.Usage = usage;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = stride * count;

    D3D11_SUBRESOURCE_DATA sub_data;
    ZeroMemory(&sub_data, sizeof(D3D11_SUBRESOURCE_DATA));
    sub_data.pSysMem = indices.data();

    auto hr = graphics->GetDevice()->CreateBuffer(&desc, &sub_data, &buffer);
    assert(SUCCEEDED(hr));
}

void IndexBuffer::Clear()
{
    SAFE_RELEASE(buffer);
    stride = 0;
    offset = 0;
    count  = 0;
}

void IndexBuffer::BindPipeline()
{
    graphics->GetDeviceContext()->IASetIndexBuffer
    (
        buffer,
        DXGI_FORMAT_R32_UINT,
        offset
    );
}

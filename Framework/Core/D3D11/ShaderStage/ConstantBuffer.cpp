#include "Framework.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(Context * context)
    : buffer(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();
}

ConstantBuffer::~ConstantBuffer()
{
    Clear();
}

void ConstantBuffer::Clear()
{
    SAFE_RELEASE(buffer);
}

void ConstantBuffer::BindPipeline(const uint & slot, const ShaderScope & scope)
{
    switch (scope)
    {
    case ShaderScope::VS: graphics->GetDeviceContext()->VSSetConstantBuffers(slot, 1, &buffer); break;
    case ShaderScope::PS: graphics->GetDeviceContext()->PSSetConstantBuffers(slot, 1, &buffer); break;
    case ShaderScope::GS: graphics->GetDeviceContext()->GSSetConstantBuffers(slot, 1, &buffer); break;
    case ShaderScope::DS: graphics->GetDeviceContext()->DSSetConstantBuffers(slot, 1, &buffer); break;
    case ShaderScope::HS: graphics->GetDeviceContext()->HSSetConstantBuffers(slot, 1, &buffer); break;
    case ShaderScope::CS: graphics->GetDeviceContext()->CSSetConstantBuffers(slot, 1, &buffer); break;
    }
}

void ConstantBuffer::Unmap()
{
    graphics->GetDeviceContext()->Unmap(buffer, 0);
}

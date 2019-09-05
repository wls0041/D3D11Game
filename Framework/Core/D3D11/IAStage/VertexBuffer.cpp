#include "Framework.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Context * context)
    : buffer(nullptr)
    , stride(0)
    , offset(0)
    , count(0)
{
    graphics = context->GetSubsystem<Graphics>();
}

VertexBuffer::~VertexBuffer()
{
    Clear();
}

auto VertexBuffer::Map() -> void *
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto result = SUCCEEDED(graphics->GetDeviceContext()->Map
	(
		buffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource
	));

	if (!result) {
		LOG_ERROR("Failed to Map");
		return nullptr;
	}
	return mappedResource.pData;
}

void VertexBuffer::Unmap()
{
	graphics->GetDeviceContext()->Unmap(buffer, 0);
}

void VertexBuffer::Clear()
{
    SAFE_RELEASE(buffer);
    stride = 0;
    offset = 0;
    count  = 0;
}

void VertexBuffer::BindPipeline()
{
    graphics->GetDeviceContext()->IASetVertexBuffers
    (
        0,
        1,
        &buffer,
        &stride,
        &offset
    );
}

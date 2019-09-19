#include "Framework.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState(Context * context)
    : state(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();
}

RasterizerState::~RasterizerState()
{
    Clear();
}

void RasterizerState::Create(const D3D11_CULL_MODE & cull_mode, const D3D11_FILL_MODE & fill_mode)
{
    D3D11_RASTERIZER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
    desc.CullMode               = cull_mode;
    desc.FillMode               = fill_mode;
    desc.DepthClipEnable        = true;
    desc.FrontCounterClockwise  = false;

    auto result = SUCCEEDED(graphics->GetDevice()->CreateRasterizerState(&desc, &state));
    if (!result)
        LOG_ERROR("Failed to create rasterizer state");
}

void RasterizerState::Clear()
{
    SAFE_RELEASE(state);
}

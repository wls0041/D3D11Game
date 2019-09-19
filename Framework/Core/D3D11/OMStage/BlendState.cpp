#include "Framework.h"
#include "BlendState.h"

BlendState::BlendState(Context * context)
    : state(nullptr)
    , blend_factor(0.0f)
{
    graphics = context->GetSubsystem<Graphics>();
}

BlendState::~BlendState()
{
    Clear();
}

void BlendState::Create(const bool & is_blend_enable, const D3D11_BLEND & src_blend, const D3D11_BLEND & dst_blend, const D3D11_BLEND_OP & blend_op, const D3D11_BLEND & src_blend_alpha, const D3D11_BLEND & dst_blend_alpha, const D3D11_BLEND_OP & blend_op_alpha, const float & blend_factor, const D3D11_COLOR_WRITE_ENABLE & color_mask)
{
    D3D11_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
    desc.AlphaToCoverageEnable                  = false;
    desc.IndependentBlendEnable                 = false;
    desc.RenderTarget[0].BlendEnable            = is_blend_enable;
    desc.RenderTarget[0].SrcBlend               = src_blend;
    desc.RenderTarget[0].DestBlend              = dst_blend;
    desc.RenderTarget[0].BlendOp                = blend_op;
    desc.RenderTarget[0].SrcBlendAlpha          = src_blend_alpha;
    desc.RenderTarget[0].DestBlendAlpha         = dst_blend_alpha;
    desc.RenderTarget[0].BlendOpAlpha           = blend_op_alpha;
    desc.RenderTarget[0].RenderTargetWriteMask  = color_mask;

    this->blend_factor = blend_factor;

    auto result = SUCCEEDED(graphics->GetDevice()->CreateBlendState(&desc, &state));
    if (!result)
        LOG_ERROR("Failed to create blend state");
}

void BlendState::Clear()
{    
    SAFE_RELEASE(state);
}
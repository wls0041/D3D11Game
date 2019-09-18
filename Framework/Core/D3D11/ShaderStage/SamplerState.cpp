#include "Framework.h"
#include "SamplerState.h"

SamplerState::SamplerState(Context * context)
    : state(nullptr)
{
    graphics = context->GetSubsystem<Graphics>();
}

SamplerState::~SamplerState()
{
    Clear();
}

void SamplerState::Create(const D3D11_FILTER & filter, const D3D11_TEXTURE_ADDRESS_MODE & address_mode, const D3D11_COMPARISON_FUNC & comparison)
{
	/*
	LODBias - Level of Detail.편차.이를 통해 밉맵의 사용 수준을 결정.멀리 있을 때 어느 밉맵을 사용하고 가까이 있을 때 어느 레벨을 사용하고
	maxanisotropy 비등방성 필터링 수준 설정
	Comparison_func 근처의 색과 비교할 때 사용.(근처 픽셀의 색 차이 등)
	borderColor 남은 공간을 채울 색
	minlod  사용하는 밉맵 수준의 수치
	*/

    D3D11_SAMPLER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
    desc.Filter         = filter;
    desc.AddressU       = address_mode;
    desc.AddressV       = address_mode;
    desc.AddressW       = address_mode;
    desc.MipLODBias     = 0.0f;
    desc.MaxAnisotropy  = 16;
    desc.ComparisonFunc = comparison;
    desc.BorderColor[0] = 0.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 0.0f;
    desc.MinLOD         = std::numeric_limits<float>::min();
    desc.MaxLOD         = std::numeric_limits<float>::max();

    auto result = SUCCEEDED(graphics->GetDevice()->CreateSamplerState(&desc, &state));
    if (!result)   
        LOG_ERROR("Failed to create sampler state");    
}

void SamplerState::Clear()
{
    SAFE_RELEASE(state);
}

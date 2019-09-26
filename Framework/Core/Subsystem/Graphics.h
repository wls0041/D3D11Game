#pragma once
#include "ISubsystem.h"

class Graphics final : public ISubsystem
{
public:
    Graphics(class Context* context);
    ~Graphics();

    auto GetDevice() const -> ID3D11Device* { return device; }
    auto GetDeviceContext() const -> ID3D11DeviceContext* { return device_context; }

    auto Initialize() -> const bool override;

	void Resize(const uint& width, const uint& height);
	void SetViewport(const uint& width, const uint& height);

    void Begin();
    void End();

private:
	void CreateSwapChain();
	void CreateRenderTargetView();
	void DeleteSurface();

private:
    ID3D11Device* device                        = nullptr;
    ID3D11DeviceContext* device_context         = nullptr;
    IDXGISwapChain* swap_chain                  = nullptr;
    ID3D11RenderTargetView* render_target_view  = nullptr;
    D3D11_VIEWPORT viewport;
    Color4 clear_color                          = Color4(0xFF555566);

    uint gpu_memory_size                        = 0;
    uint numerator                              = 0;
    uint denominator                            = 0;
    std::wstring gpu_description                = L"";
};
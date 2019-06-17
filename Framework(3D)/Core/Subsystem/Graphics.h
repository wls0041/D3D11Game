#pragma once

class Graphics final
{
public:
    Graphics();
    ~Graphics();

    auto GetDevice() const -> ID3D11Device* { return device; }
    auto GetDeviceContext() const -> ID3D11DeviceContext* { return device_context; }

    void Initialize();
    
    void Begin();
    void End();

private:
    void CreateBackBuffer(const uint& width, const uint& height);

private:
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    IDXGISwapChain* swap_chain;

    ID3D11RenderTargetView* rtv;
    ID3D11DepthStencilView* dsv;
    D3D11_VIEWPORT viewport;
    D3DXCOLOR clear_color;
};
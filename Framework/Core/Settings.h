#pragma once
#include "Framework.h"

class Settings final
{
public:
    static Settings& Get()
    {
        static Settings instance;
        return instance;
    }

    auto GetWindowInstance() const -> HINSTANCE { return hInstance; }
    void SetWindowInstance(HINSTANCE hInstance) { this->hInstance = hInstance; }

    auto GetWindowHandle() const -> HWND { return handle; }
    void SetWindowHandle(HWND handle) { this->handle = handle; }

    auto GetWidth() const -> const float { return windowSize.x; }
    void SetWidth(const float& width) { this->windowSize.x = width; }

    auto GetHeight() const -> const float { return windowSize.y; }
    void SetHeight(const float& height) { this->windowSize.y = height; }

	auto IsVsync() const -> const bool { return is_vsync; }
	void SetVsync(const bool& is_vsync) { this->is_vsync = is_vsync; }

private:
    Settings() = default;
    ~Settings() = default;

private:
    HINSTANCE hInstance = nullptr;
    HWND handle         = nullptr;
    Vector2 windowSize  = Vector2(0, 0);
    bool is_vsync       = true;
};
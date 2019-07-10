#pragma once
#include "Framework.h"

namespace Window
{
    static HINSTANCE Instance;
    static HWND Handle;

	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> Input_proc;
	static std::function<LRESULT(HWND, uint, WPARAM, LPARAM)> Editor_proc; //imgui에서 쓸 용도

    inline LRESULT CALLBACK WndProc
    (
        HWND handle,
        uint message,
        WPARAM wParam,
        LPARAM lParam
    )
    {
        if (Input_proc != nullptr)
            Input_proc(handle, message, wParam, lParam);

		if (Editor_proc != nullptr)
			Editor_proc(handle, message, wParam, lParam);

        switch (message)
        {
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(handle, message, wParam, lParam);
        }

        return 0;
    }

    inline void Create
    (
        HINSTANCE instance,
        const uint& width,
        const uint& height
    )
    {
        Instance = instance;

        WNDCLASSEX wnd_class;
        wnd_class.cbClsExtra    = 0;
        wnd_class.cbWndExtra    = 0;
        wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wnd_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wnd_class.hIcon         = LoadCursor(nullptr, IDI_WINLOGO);
        wnd_class.hIconSm       = LoadCursor(nullptr, IDI_WINLOGO);
        wnd_class.hInstance     = instance;
        wnd_class.lpfnWndProc   = static_cast<WNDPROC>(WndProc);
        wnd_class.lpszClassName = L"D3D11Game";
        wnd_class.lpszMenuName  = nullptr;
        wnd_class.style         = CS_HREDRAW | CS_VREDRAW;
        wnd_class.cbSize        = sizeof(WNDCLASSEX);

        auto check = RegisterClassEx(&wnd_class);
        assert(check != 0);

        Handle = CreateWindowExW
        (
            WS_EX_APPWINDOW,
            L"D3D11Game",
            L"D3D11Game",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<int>(width),
            static_cast<int>(height),
            nullptr,
            nullptr,
            instance,
            nullptr            
        );
        assert(Handle != nullptr);
    }

    inline void Show()
    {
        SetForegroundWindow(Handle);
        SetFocus(Handle);
        ShowCursor(TRUE);
        ShowWindow(Handle, SW_SHOWNORMAL);
        UpdateWindow(Handle);
    }

    inline const bool Update()
    {
        MSG msg;
        ZeroMemory(&msg, sizeof(MSG));

        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return msg.message != WM_QUIT;
    }

    inline void Destroy()
    {
        DestroyWindow(Handle);
        UnregisterClass(L"D3D11Game", Instance);
    }

    inline const uint GetWidth()
    {
        RECT rect;
        GetClientRect(Handle, &rect);
        return static_cast<uint>(rect.right - rect.left);
    }

    inline const uint GetHeight()
    {
        RECT rect;
        GetClientRect(Handle, &rect);
        return static_cast<uint>(rect.bottom - rect.top);
    }
}
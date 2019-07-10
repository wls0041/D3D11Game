#include "stdafx.h"
#include "Editor.h"
#include "./Core/Window.h"
#include "./Imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM); //외부의 접근 차단하려면 static으로

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window::Create(hInstance, 1280, 720);
    Window::Show();

    Settings::Get().SetWindowInstance(Window::Instance);
    Settings::Get().SetWindowHandle(Window::Handle);
    Settings::Get().SetWidth(static_cast<float>(Window::GetWidth()));
    Settings::Get().SetHeight(static_cast<float>(Window::GetHeight()));

    auto editor = std::make_unique<Editor>();

	Window::Input_proc = Input::MouseProc;
	Window::Editor_proc = ImGui_ImplWin32_WndProcHandler;

    while (Window::Update())
    {
        editor->Update();
        editor->Render();
    }

    Window::Destroy();
    return 0;
}
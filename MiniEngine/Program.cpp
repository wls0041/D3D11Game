#include "stdafx.h"
#include "Editor.h"
#include "./Core/Window.h"
#include "./Imgui/imgui_impl_win32.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM); //�ܺ��� ���� �����Ϸ��� static����

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window::Create(hInstance, 1280, 720);
    Window::Show();

    Settings::Get().SetWindowInstance(Window::Instance);
    Settings::Get().SetWindowHandle(Window::Handle);
    Settings::Get().SetWidth(static_cast<float>(Window::GetWidth()));
    Settings::Get().SetHeight(static_cast<float>(Window::GetHeight()));
	Settings::Get().SetVsync(false);

    auto editor = std::make_unique<Editor>();

	Window::Input_proc  = Input::MouseProc;
	Window::Editor_proc = ImGui_ImplWin32_WndProcHandler;
	Window::Resize		= [&editor](const uint& width, const uint& height)
	{
		if (editor)
			editor->Resize(width, height);
	};

    while (Window::Update())
    {
        editor->Update();
        editor->Render();
    }

    Window::Destroy();
    return 0;
}
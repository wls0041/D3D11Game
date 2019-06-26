#include "stdafx.h"
#include "Editor.h"
#include "./Core/Window.h"

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

    while (Window::Update())
    {
        editor->Update();
        editor->Render();
    }

    Window::Destroy();
    return 0;
}
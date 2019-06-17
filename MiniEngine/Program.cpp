#include "stdafx.h"
#include "./Core/Window.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    Window::Create(hInstance, 1280, 720);
    Window::Show();

    while (Window::Update())
    {

    }

    Window::Destroy();
    return 0;
}
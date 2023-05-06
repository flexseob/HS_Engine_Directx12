#include "Window.h"

#include <iostream>
#include "Context.h"
#include "imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void HS_Engine::Window::Update()
{
    ShowWindow(m_windowData.m_hwnd, m_windowData.m_nCmdShow);

    m_windowData.m_context->Update();
    m_windowData.m_context->Render();


}

void HS_Engine::Window::Shutdown()
{
}

LRESULT HS_Engine::Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) {
            if (MessageBox(0, L"Are you sure you want to exit?",
                L"HS_Engine Quit?", MB_YESNO | MB_ICONQUESTION) == IDYES)
                DestroyWindow(hWnd);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}

bool HS_Engine::Window::Init(const WindowProperties& window_properties)
{
	m_windowData.m_width = window_properties.m_width;
	m_windowData.m_height = window_properties.m_height;
	m_windowData.m_hinstance = window_properties.m_hinstance;
	m_windowData.m_windowTitle = window_properties.m_title;
    m_windowData.m_nCmdShow = window_properties.m_nCmdShow;
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = window_properties.m_hinstance;
    windowClass.hCursor = LoadCursor(window_properties.m_hinstance, IDC_ARROW);
    windowClass.lpszClassName = m_windowData.m_windowName;
	
   

    if (!RegisterClassEx(&windowClass))
    {
        MessageBox(NULL, L"Error registering class",
            L"Error", MB_OK | MB_ICONERROR);

        return false;
    }

    m_windowData.m_hwnd = CreateWindowEx(NULL,
        m_windowData.m_windowName,
        m_windowData.m_windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        m_windowData.m_width, m_windowData.m_height,
        NULL,
        NULL,
        m_windowData.m_hinstance,
        NULL);

    m_windowData.m_context = std::make_shared<HS_Engine::Context>();
    m_windowData.m_context->Init();
    ShowWindow(m_windowData.m_hwnd, m_windowData.m_nCmdShow);
}



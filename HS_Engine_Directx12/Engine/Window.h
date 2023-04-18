#pragma once
#include <string>

#include "stdafx.h"
#include <memory>
struct WindowProperties
{
	std::wstring m_title;
	unsigned m_width;
	unsigned m_height;
	HINSTANCE m_hinstance;
	int m_nCmdShow = 0;
	WindowProperties(HINSTANCE hinstance,int nCmdShow, const std::wstring& title = L"HS_Engine DirectX v2.0", uint32_t width = 1600, uint32_t height = 900)
		:m_hinstance(hinstance),m_nCmdShow(nCmdShow), m_title(title), m_width(width), m_height(height) {}
};

namespace HS_Engine
{
	class Context;

	class Window
	{
	private:
		struct WindowData
		{
			HWND m_hwnd = nullptr;
			HINSTANCE m_hinstance = nullptr;
			LPCTSTR m_windowName = L"DirectX";
			std::wstring m_windowTitle;
			int m_width = 0;
			int m_height = 0;
			bool m_isFullScreen = false;
			int m_nCmdShow;
			std::shared_ptr<Context> m_context;
		};

	public:
		Window() =default;
		Window(const WindowProperties& window_properties)
		{
			Init(window_properties);
		}
		void Update();
		void Shutdown();
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		WindowData& GetWindowData() { return m_windowData; }
		bool Init(const WindowProperties& window_properties);

	private:
		WindowData m_windowData;
	};
}
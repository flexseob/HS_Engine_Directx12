#ifndef ENGINE_H
#define ENGINE_H
#include <chrono>
#include "stdafx.h"
#include "Window.h"
#include "ImguiManager.h"
namespace HS_Engine
{
	class Engine
	{
	public:
		Engine();
		double deltaTime = 0;
		std::chrono::time_point<std::chrono::high_resolution_clock> pastTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> nowTime;

		static bool mIsSceneEnded;

		void Init(HINSTANCE hinstance, int nCmdShow);
		void Update();
		void Unload();
		void Cleanup();
		bool IsSceneEnded() const;

		static void MakeSceneEnded()
		{
			mIsSceneEnded = true;
		}
		float m_FPS = 144;

		static Engine& Instance() { static Engine Instance; return Instance; }
		HINSTANCE GetInstance() { return m_hinstance; }

		static ImGuiManager& GetImguiManager() { return Instance().mImguiManager; }

		static HS_Engine::Window* GetWindow()
		{
			return mWindow.get();
		}
	private:
		static std::unique_ptr<Window> mWindow;
		ImGuiManager mImguiManager;
		HINSTANCE m_hinstance;
	};
}

#endif//!ENGINE_H
#include "Engine.h"
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
namespace HS_Engine
{
	std::unique_ptr<Window> Engine::mWindow = std::make_unique<Window>();
	Engine::Engine()
	{
	
	}
	static const WORD MAX_CONSOLE_LINES = 500;
	void RedirectIOToConsole()
	{
		int hConHandle;
		long lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE* fp;

		// allocate a console for this app
		AllocConsole();

		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "w");

		*stdout = *fp;

		setvbuf(stdout, NULL, _IONBF, 0);

		// redirect unbuffered STDIN to the console

		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "r");
		*stdin = *fp;
		setvbuf(stdin, NULL, _IONBF, 0);

		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);

		fp = _fdopen(hConHandle, "w");

		*stderr = *fp;

		setvbuf(stderr, NULL, _IONBF, 0);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		std::ios::sync_with_stdio();
	}
	void Engine::Init(HINSTANCE hinstance, int nCmdShow)
	{

		fprintf(stdout, "Test output to stdout\n");


		std::cout << "HS(HoSeob) Engine Init" << std::endl;
		mWindow->Init(WindowProperties(hinstance, nCmdShow));
		m_hinstance = hinstance;
		
	}

	void Engine::Update()
	{
		MSG msg = {};
		while (!IsSceneEnded())
		{
			nowTime = std::chrono::high_resolution_clock::now();
			const std::chrono::duration deltaTick = nowTime - pastTime;
			deltaTime = std::chrono::duration<double>(deltaTick).count();
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (1 / m_FPS <= deltaTime)
			{
				//std::cout << GetWindow()->GetWindowData().m_width << ", " << GetWindow()->GetWindowData().m_height << std::endl;
				mWindow->Update();
				pastTime = nowTime;
			}
		}
	}

	void Engine::Unload()
	{
#ifdef _DEBUG
		FreeConsole();
#endif
	}

	void Engine::Cleanup()
	{

	}

	bool Engine::IsSceneEnded() const
	{
		return false;
	}
}
#include "Engine/stdafx.h"
#include <iostream>
#include "Engine/Engine.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    HS_Engine::Engine& engine = HS_Engine::Engine::Instance();
	try
	{
#ifdef _DEBUG
		//RedirectIOToConsole();
#endif
		engine.Init(hInstance,nCmdShow);
		//engine.Instance().GetSceneManager().AddScene(ocean_rendering);
		engine.Update();
		engine.Unload();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
    return 0;
}
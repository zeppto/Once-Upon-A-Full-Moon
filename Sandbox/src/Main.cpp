#include "Game.hpp"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	if (!glfwInit())
	{
		// Initialization failed
	}

	auto app = new Game();
	app->Run();
	delete app;

	system("pause");
	glfwTerminate();
	return 0;
}

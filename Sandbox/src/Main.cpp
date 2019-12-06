#include <mcspch.hpp>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.hpp"

int main(int argc, char** argv)
{
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	srand((unsigned int)time(NULL));

	auto app = FY_NEW MCS::Game();
	app->Run();
	delete app;

	return 0;
}
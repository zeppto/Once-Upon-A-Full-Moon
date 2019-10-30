#include <mcspch.hpp>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.hpp"


int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	srand(time(NULL));

	//glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
	//glm::vec3 dest = glm::vec3(10.0f, 0.0f, 10.0f);
	//glm::vec3 dir = origin - dest;

	//float dist = glm::length(origin);

	auto app = FY_NEW MCS::Game();
	app->Run();
	delete app;

	return 0;
}

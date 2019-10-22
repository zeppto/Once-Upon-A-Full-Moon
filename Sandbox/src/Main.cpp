#include <mcspch.hpp>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Game.hpp"


int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 P = glm::vec3(-10.0f, 0.0f, 0.0f);
	glm::vec3 Q = glm::vec3(0.0f, 0.0f, 10.0f);

	glm::vec3 dirP = glm::normalize(P - origin);
	glm::vec3 dirQ = glm::normalize(Q - origin);

	float product = glm::dot(dirP, dirQ);
	float a = glm::acos(product);
	float rad = glm::degrees(a);

	auto app = FY_NEW MCS::Game();
	app->Run();
	delete app;

	return 0;
}

#include <mcspch.hpp>
#include "Game.hpp"

namespace MCS
{
	Game::Game()
	{
		PushLayer(FY_NEW InspectorLayer());
	}

	Game::~Game()
	{

	}
}

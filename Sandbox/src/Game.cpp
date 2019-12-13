#include <mcspch.hpp>
#include "Game.hpp"
#include "States/MenuState.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& state = Application::Get().GetStateMachine();
		state.AddState(Frosty::StateRef(FY_NEW(MenuState)));


//#ifdef FY_DEBUG
		PushLayer(FY_NEW InspectorLayer());
//#else
//		Application::Get().StartGame(true);
//#endif
	}

	Game::~Game()
	{
	}
}
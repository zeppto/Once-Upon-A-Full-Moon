#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "Frosty/StateMachine/State.hpp"
#include "Frosty/Core/Application.hpp"
#include "OptionMenuState.h"
#include "PlayingState.h"

namespace Frosty
{
	class MainMenuState:public State {
	public:
		MainMenuState(Application * app); 
		~MainMenuState();

		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
		Application *app;
	};
}

#endif // !MAIN_MENU_STATE_HPP
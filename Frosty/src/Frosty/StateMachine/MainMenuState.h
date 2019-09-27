#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "Frosty/StateMachine/State.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	class MainMenuState:public State {
	public:
		MainMenuState(Application * app); 
		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
		Application *app;
	};
}

#endif // !MAIN_MENU_STATE_HPP
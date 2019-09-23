#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "Frosty/StateMachine/State.hpp"

namespace Frosty
{
	class MainMenuState:public State {
	public:
		MainMenuState();
		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
	};
}

#endif // !MAIN_MENU_STATE_HPP
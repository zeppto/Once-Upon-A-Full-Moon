#ifndef MAIN_MENU_STATE_HPP
#define MAIN_MENU_STATE_HPP

#include "State.hpp"

namespace MCS
{
	class MainMenuState:public State {
	public:
		MainMenuState(Frosty::Application * app); 
		~MainMenuState();

		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
		Frosty::Application *app;
	};
}

#endif // !MAIN_MENU_STATE_HPP
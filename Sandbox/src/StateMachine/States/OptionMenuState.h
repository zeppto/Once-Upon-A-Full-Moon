#ifndef OPTION_MENU_STATE_HPP
#define OPTION_MENU_STATE_HPP

#include "State.hpp"
//~~ I DO NOT like having to include application in more than one state...Especially not as main menu already includes this include.
#include "Frosty/Core/Application.hpp"

namespace MCS
{
	class OptionMenuState :public State 
	{
	public:
		OptionMenuState();
		~OptionMenuState();

		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
		//Temp. I have no idea what we actually wanna have in here.
		int volume;
		int brightness;
		bool fullscreen;

		Frosty::Application *app;

	};
}


#endif // !OPTION_MENU_STATE_HPP

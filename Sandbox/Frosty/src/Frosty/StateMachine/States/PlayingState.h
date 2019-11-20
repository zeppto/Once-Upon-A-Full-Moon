#ifndef PLAYING_STATE_HPP
#define PLAYING_STATE_HPP

#include "Frosty/StateMachine/State.hpp"

//~~ The state the game goes to once Play is pressed from the main menu.
namespace Frosty
{
	class PlayingState :public State
	{
	public:
		PlayingState();
		~PlayingState();

		void Initiate();
		void OnInput();
		void OnUpdate();
	private:
		//~~ TODO: Add helpful variables here.

	};
}


#endif // !PLAYING_STATE_HPP

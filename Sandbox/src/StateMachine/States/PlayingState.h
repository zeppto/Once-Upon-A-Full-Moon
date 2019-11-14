#ifndef PLAYING_STATE_HPP
#define PLAYING_STATE_HPP
#include "State.hpp"

namespace MCS
{
	class PlayingState :public State
	{
	private:


	public:
		PlayingState();
		~PlayingState();

		void Initiate();
		void OnInput();
		void OnUpdate();
		

	};
}


#endif // !PLAYING_STATE_HPP

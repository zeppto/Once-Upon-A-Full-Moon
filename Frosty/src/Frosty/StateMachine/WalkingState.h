#ifndef WALKING_STATE_HPP
#define WALKING_STATE_HPP

#include "State.hpp"
#include <string>

namespace Frosty
{
	class WalkingState :public State {
	public:
		WalkingState(std::string kickAnimName);
		~WalkingState();

		void Initiate(std::string kickAnimName);
		void OnInput();
		void OnUpdate();

		std::string* getName();
	private:
		std::string animName;
	};
}

#endif WALKING_STATE_HPP

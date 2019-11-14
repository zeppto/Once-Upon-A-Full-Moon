#ifndef KICKIN_STATE_HPP
#define KICKIN_STATE_HPP

#include "State.hpp"

namespace Frosty
{
	class KickingState :public State {
	public:
		KickingState(std::string kickAnimName);
		~KickingState();

		void Initiate(std::string kickAnimName);
		void OnInput();
		void OnUpdate();

		std::string* getName();
	private:
		std::string animName;
	};
}

#endif KICKIN_STATE_HPP

#ifndef STATE_HPP
#define STATE_HPP
#include "Frosty/Core/Application.hpp"

namespace MCS
{
	class State
	{
	public:
		virtual ~State() { }

		virtual void Initiate() = 0;
		virtual void OnInput() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnLastUpdate() { }
		
		virtual void Pause() { }
		virtual void Resume() { }
	};
}
#endif // !STATE_HPP

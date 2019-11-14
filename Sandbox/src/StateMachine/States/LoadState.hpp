#ifndef  LOADSTATE_HPP
#define LOADSTATE_HPP
#include "StateMachine/States/State.hpp"

namespace MCS
{
	class LoadState : public State
	{

	private:

		bool m_IsDone;
		Frosty::Application* m_App;

	public:

		inline LoadState(Frosty::Application* App) : m_IsDone(false), m_App(App) { Initiate(); }

		void Initiate();
		void OnInput();
		void OnUpdate();



	};


}


#endif // ! LOADSTATE_HPP


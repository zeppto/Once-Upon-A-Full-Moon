#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include "State.hpp"

namespace Frosty
{
	typedef std::unique_ptr<State> StateRef;

	class StateMachine
	{
	public:
		StateMachine() { }
		virtual ~StateMachine();
		StateMachine(const StateMachine& obj) = delete;

		StateMachine& operator=(const StateMachine& obj) = delete;

		StateRef& GetActiveState() { return m_States.top(); }

		// Replacing means it will pop the current state and add the new one.
		// Otherwise it will just pause the current state and add another on top of the stack
		void AddState    (StateRef newState, bool isReplacing = true);
		void RemoveState();
		void ClearAllStates();

		bool ProcessStateChanges();

	private:
		void Clear();

		std::stack<StateRef> m_States;
		StateRef m_NewState;

		bool m_IsRemoving = false;
		bool m_IsAdding = false;
		bool m_IsReplacing = false;
		bool m_ClearAll = false;
	};
}
#endif // !STATE_MACHINE_HPP

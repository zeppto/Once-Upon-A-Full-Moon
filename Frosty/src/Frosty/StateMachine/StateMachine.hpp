#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP
#include "State.hpp"

namespace Frosty
{
	typedef std::unique_ptr<State> StateRef;

	class StateMachine
	{
	public:
		StateMachine() = default;
		virtual ~StateMachine();
		StateMachine(const StateMachine& obj) = delete;

		StateMachine& operator=(const StateMachine& obj) = delete;

		static StateRef& GetActiveState() { return m_States.top(); }

		// Replacing means it will pop the current state and add the new one.
		// Otherwise it will just pause the current state and add another on top of the stack
		static void AddState(StateRef newState, bool isReplacing = true);
		static void RemoveState();
		static void ClearAllStates();

		static bool ProcessStateChanges();

	private:
		static void Clear();

		static std::stack<StateRef> m_States;
		static StateRef m_NewState;

		static bool m_IsRemoving;
		static bool m_IsAdding;
		static bool m_IsReplacing;
		static bool m_ClearAll;
	};
}
#endif // !STATE_MACHINE_HPP

#include <fypch.hpp>
#include "StateMachine.hpp"

namespace Frosty
{
	std::stack<Frosty::StateRef> StateMachine::m_States;
	Frosty::StateRef StateMachine::m_NewState;
	bool StateMachine::m_IsRemoving = false;
	bool StateMachine::m_IsAdding = false;
	bool StateMachine::m_IsReplacing = false;
	bool StateMachine::m_ClearAll = false;

	StateMachine::~StateMachine()
	{		
		Clear();
	}

	void StateMachine::AddState(StateRef newState, bool isReplacing)
	{
		m_IsAdding = true;
		m_IsReplacing = isReplacing;
		m_NewState = std::move(newState);
	}
	
	void StateMachine::RemoveState()
	{
		m_IsRemoving = true;
	}

	void StateMachine::ClearAllStates()
	{
		m_ClearAll = true;
	}
	
	bool StateMachine::ProcessStateChanges()
	{
		// Removing State - Check if stack is empty
		if (m_IsRemoving && !m_States.empty())
		{
			// Not empty - Remove top state
			m_States.pop();
			// Check again if stack is empty
			if (!m_States.empty())
			{
				// Not empty - Resume top state
				m_States.top()->Resume();
			}

			// Done with removal
			m_IsRemoving = false;
			return false;
		}

		// Check if we want to add a new state
		if (m_IsAdding)
		{
			// Check if stack is empty
			if (!m_States.empty())
			{
				// Not empty - check if we want to replace or not 
				if (m_IsReplacing)
				{
					// Replacing - Remove the top state
					m_States.pop();
				}
				else
				{
					// Not replacing - Pause top state
					m_States.top()->Pause();
				}
			}

			// Check to see if we want to clear all states before adding a new one
			if (m_ClearAll)
			{
				Clear();
			}

			// Add new state on top of the stack and initialize
			m_IsAdding = false;
			m_ClearAll = false;
			m_States.push(std::move(m_NewState));
			m_States.top()->Initiate();
			return true;
		}

		return false;
	}

	void StateMachine::Clear()
	{
		while (!m_States.empty())
		{
			m_States.pop();
		}
	}
}

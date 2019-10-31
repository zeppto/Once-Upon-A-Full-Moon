#ifndef KEY_EVENT_HPP
#define KEY_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	class KeyEvent : public BaseEvent
	{
	public:
		inline int GetKeyCode() const { return m_KeyCode; }

	protected:
		KeyEvent(int keycode) : m_KeyCode(keycode) { }

		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatcount) : KeyEvent(keycode), m_RepeatCount(repeatcount) { }

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_TYPE(KeyPressed)

	private:
		int m_RepeatCount;

	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_TYPE(KeyReleased)

	};
}

#endif // !KEY_EVENT_HPP

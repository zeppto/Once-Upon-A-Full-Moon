#ifndef MOUSE_EVENT_HPP
#define MOUSE_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	class MouseMovedEvent : public BaseEvent
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const {
			return m_MouseX;
		}
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_TYPE(MouseMoved)
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public BaseEvent
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_TYPE(MouseScrolled)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public BaseEvent
	{
	public:
		inline int GetMouseButton() const { return m_Button; }

	protected:
		MouseButtonEvent(int button) : m_Button(button) { }

		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_TYPE(MouseButtonPressed)

	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_TYPE(MouseButtonReleased)

	};
}

#endif // !MOUSE_EVENT_HPP

#ifndef APPLICATION_EVENT_HPP
#define APPLICATION_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	class WindowCloseEvent : public BaseEvent
	{
	public:
		WindowCloseEvent() { }

		EVENT_TYPE(WindowClose)
	};

	class WindowMaximizeEvent : public BaseEvent
	{
	public:
		WindowMaximizeEvent(int maximized) : m_Maximized(maximized) { }

		inline bool IsMaximized() const { return m_Maximized; }

		EVENT_TYPE(WindowMaximized)
	private:
		int m_Maximized;
	};

	// TODO: Should this store previous size?
	class WindowResizeEvent : public BaseEvent
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_TYPE(WindowResize)
	private:
		unsigned int m_Width, m_Height;
	};

	class WindowFocusEvent : public BaseEvent
	{
	public:
		WindowFocusEvent(int focused)
			: m_Focused(focused) {}

		inline bool IsFocused() const { return m_Focused; }


		EVENT_TYPE(WindowFocus)
	private:
		int m_Focused;
	};

	class WindowMovedEvent : public BaseEvent
	{
	public:
		WindowMovedEvent(int xpos, int ypos)
			: m_XPos(xpos), m_YPos(ypos) {}

		inline int GetXPos() const { return m_XPos; }
		inline int GetYPos() const { return m_YPos; }


		EVENT_TYPE(WindowMoved)
	private:
		int m_XPos, m_YPos;
	};
}

#endif // !APPLICATION_EVENT_HPP

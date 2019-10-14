#ifndef EVENT_SYSTEM_HPP
#define EVENT_SYSTEM_HPP

#include <fypch.hpp>

namespace Frosty
{
	using EventId = size_t;
	using ButtonID = size_t;

	enum class EventType
	{
		None = 0,
		WindowClose, WindowMaximized, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		ButtonClicked
	};

#define EVENT_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

	class BaseEvent
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }
	};


#pragma region WindowEvents

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

#pragma endregion

#pragma region KeyEvents

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

#pragma endregion

#pragma region MouseEvents

	class MouseMovedEvent : public BaseEvent
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const {
			return m_MouseX; }
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

#pragma endregion

#pragma region ButtonEvents

	struct ButtonEvent : public BaseEvent
	{
		ButtonID ButtonId;

		ButtonEvent(ButtonID bid) : ButtonId(bid) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "ButtonEvent: " << ButtonId;
			return ss.str();
		}

		EVENT_TYPE(ButtonClicked)
	};

#pragma endregion

#pragma region System

	class BaseSubscriber
	{
	public:
		BaseSubscriber() = default;
		virtual ~BaseSubscriber() { }

		virtual void CallFunction(BaseEvent& event) = 0;
	};

	template<class SystemType, class EventType>
	class Subscriber : public BaseSubscriber
	{
	public:
		typedef void(SystemType::*MemberFunction)(EventType&);

	public:
		Subscriber(SystemType * instance, MemberFunction function) : m_Instance(instance), m_Function(function) { }
		virtual ~Subscriber() { }

		inline void CallFunction(BaseEvent& event)
		{
			// Cast event to the correct type and call member function
			(m_Instance->*m_Function)(event);
		}

	private:
		SystemType* m_Instance;
		MemberFunction m_Function;

	};

	class EventBus
	{
	private:
		EventBus() { }

	public:
		virtual ~EventBus() { }

		template <typename EventType>
		inline void Publish(EventType& event)
		{
			///	///////////////////////////////////////////////////////////	///
			///						Order of business						///
			///	1.	Make sure event inherits from BaseEvent					///
			///	2.	Loop through all subscribers and call the function		///
			///	///////////////////////////////////////////////////////////	///


			// Step 1
			static_assert(std::is_base_of<BaseEvent, EventType>::value,
				"EventType must inherit from BaseEvent");

			// Step 2
			for (auto & sub : m_Subscribers)
			{
				sub->CallFunction(event);
			}
		}

		template<class SystemType, class EventType>
		inline void Subscribe(SystemType* instance, void (SystemType::*function)(EventType &))
		{
			///	///////////////////////////////////////////////////////////	///
			///						Order of business						///
			///	1.	Make sure event inherits from BaseEvent					///
			///	2.	Add subscriber to the correct subscriber list			///
			///	///////////////////////////////////////////////////////////	///


			// Step 1
			static_assert(std::is_base_of<BaseEvent, EventType>::value,
				"EventType must inherit from BaseEvent");

			// Step 2
			m_Subscribers.emplace_back(FY_NEW Subscriber<SystemType, EventType>(instance, function));

		}

		inline static EventBus* GetEventBus()
		{
			if (s_Instance == nullptr)
			{
				s_Instance = FY_NEW EventBus();
			}

			return s_Instance;
		}

	private:
		inline static void Delete() { delete s_Instance; }

	private:
		static EventBus* s_Instance;
		std::list<std::shared_ptr<BaseSubscriber>> m_Subscribers;

		friend class Application;

	};

	inline std::ostream& operator<<(std::ostream& os, const BaseEvent& e)
	{
		return os << e.ToString();
	}

#pragma endregion
}

#endif // !EVENT_SYSTEM_HPP

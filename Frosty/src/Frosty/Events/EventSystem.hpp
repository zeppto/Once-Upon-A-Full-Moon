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
		ArrowHit, Dash, CheckCollision, Collision, BasicAttack, BaitPlaced, ExitCurrentLevel,
		SaveLevel, CreateLevel
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

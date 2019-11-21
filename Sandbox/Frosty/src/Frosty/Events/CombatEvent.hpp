#ifndef COMBAT_EVENT_HPP
#define COMBAT_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	namespace ECS { struct Entity; }

	class ArrowHitEvent : public BaseEvent
	{
		ArrowHitEvent() { }

		EVENT_TYPE(ArrowHit)

	private:
		std::shared_ptr<ECS::Entity> m_Attacker;

	};
}

#endif // !COMBAT_EVENT_HPP

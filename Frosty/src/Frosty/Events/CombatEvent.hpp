#ifndef COMBAT_EVENT_HPP
#define COMBAT_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	struct ECS::Entity;

	class ArrowHitEvent : public BaseEvent
	{
		ArrowHitEvent(std::shared_ptr<ECS::Entity> attack) { }

		std::shared_ptr<ECS::Entity> getAttack() { return m_Attacker; };

		EVENT_TYPE(ArrowHit)

	private:
		std::shared_ptr<ECS::Entity> m_Attacker;

	};

	class NormalAttackEvent : public BaseEvent
	{
		NormalAttackEvent() { }

		EVENT_TYPE(ArrowHit)

	private:
		std::shared_ptr<ECS::Entity> m_Attacker;

	};
}

#endif // !COMBAT_EVENT_HPP

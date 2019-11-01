#ifndef COMBAT_EVENT_HPP
#define COMBAT_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	struct ECS::Entity;

	class ArrowHitEvent : public BaseEvent
	{
	public:
		ArrowHitEvent(const std::shared_ptr<ECS::Entity> attack) : m_Attacker(attack) { }

		std::shared_ptr<ECS::Entity> getAttack() { return m_Attacker;  }

		EVENT_TYPE(ArrowHit)

	private:
		std::shared_ptr<ECS::Entity> m_Attacker;

	};

	class NormalAttackEvent : public BaseEvent
	{
	public:
		NormalAttackEvent(const std::shared_ptr<ECS::Entity> player) : m_Player(player) { }

		std::shared_ptr<ECS::Entity> getPlayer() { return m_Player; }

		EVENT_TYPE(NormalHit)

	private:
		std::shared_ptr<ECS::Entity> m_Player;
	};

	class AreaAttackEvent : public BaseEvent
	{
	public:
		AreaAttackEvent(const std::shared_ptr<ECS::Entity> player) : m_Player(player) { }

		std::shared_ptr<ECS::Entity> getPlayer() { return m_Player; }

		EVENT_TYPE(AreaHit)

	private:
		std::shared_ptr<ECS::Entity> m_Player;
	};

	class StrongAttackEvent : public BaseEvent
	{
	public:
		StrongAttackEvent(const std::shared_ptr<ECS::Entity> player) : m_Player(player) { }

		std::shared_ptr<ECS::Entity> getPlayer() { return m_Player; }

		EVENT_TYPE(StrongHit)

	private:
		std::shared_ptr<ECS::Entity> m_Player;
	};
}

#endif // !COMBAT_EVENT_HPP

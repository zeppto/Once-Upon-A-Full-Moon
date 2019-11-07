#ifndef ABILITY_EVENT_HPP
#define ABILITY_EVENT_HPP

#include "EventSystem.hpp"

namespace Frosty
{
	namespace ECS { struct Entity; }

	class CollisionEvent : public BaseEvent
	{
	public:
		CollisionEvent(const std::shared_ptr<ECS::Entity>& entityA, const std::shared_ptr<ECS::Entity>& entityB, const glm::vec3& offset)
			: m_EntityA(entityA), m_EntityB(entityB), m_Offset(offset) { }

		const std::shared_ptr<ECS::Entity>& GetEntityA() const { return m_EntityA; }
		const std::shared_ptr<ECS::Entity>& GetEntityB() const { return m_EntityB; }
		std::shared_ptr<ECS::Entity>& GetEntityA() { return m_EntityA; }
		std::shared_ptr<ECS::Entity>& GetEntityB() { return m_EntityB; }
		const glm::vec3& GetOffset() const { return m_Offset; }

		EVENT_TYPE(Collision)

	private:
		std::shared_ptr<ECS::Entity> m_EntityA;
		std::shared_ptr<ECS::Entity> m_EntityB;
		glm::vec3 m_Offset{ 0.0f };
	};

	class BasicAttackEvent : public BaseEvent
	{
	public:
		BasicAttackEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(BasicAttack)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class ExitLevelEvent : public BaseEvent
	{
	public:
		ExitLevelEvent(const std::shared_ptr<ECS::Entity>& exit, const std::shared_ptr<ECS::Entity>& player) : m_ExitEntity(exit), m_PlayerEntity(player){ }

		const std::shared_ptr<ECS::Entity>& GetExitEntity() const { return m_ExitEntity; }
		const std::shared_ptr<ECS::Entity>& GetPlayerEntity() const { return m_PlayerEntity; }

		EVENT_TYPE(ExitCurrentLevel)

	private:
		std::shared_ptr<ECS::Entity> m_ExitEntity;
		std::shared_ptr<ECS::Entity> m_PlayerEntity;
	};
}

#endif // !ABILITY_EVENT_HPP

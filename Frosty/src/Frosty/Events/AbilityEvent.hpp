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
}

#endif // !ABILITY_EVENT_HPP

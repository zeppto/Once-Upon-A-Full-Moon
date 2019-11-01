#ifndef COMBATSYSTEM_HPP
#define COMBATSYSTEM_HPP
#include "Frosty.h"
#include "Frosty/Events/CombatEvent.hpp"
#include "Frosty/API/AssetManager.hpp"

class CombatSystem : public Frosty::ECS::BaseSystem
{
public:
	CombatSystem() = default;
	virtual ~CombatSystem() = default;

	virtual void Init() override;

	virtual void OnInput() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	//events
	void OnEvent(Frosty::BaseEvent& e);
	void OnNormalAttackHitEvent(Frosty::NormalAttackEvent& e);
	void OnAreaAttackHitEvent(Frosty::AreaAttackEvent& e);
	void OnStrongAttackHitEvent(Frosty::StrongAttackEvent& e);

private:
	bool Collision(glm::vec3 posA, glm::vec3 scaleA, float centerA[3], float halfSizeA[3], glm::vec3 posB, glm::vec3 scaleB, float centerB[3],
		float  halfSizeB[3], glm::vec3 hitboxPos = glm::vec3(0.0f) , float reach = 1);
	void OnHit(float* hitHp, float damage, glm::vec3* hitPos, glm::vec3 attackerPos);
	void createArrow(glm::vec3 spwanPos, glm::vec3 rotation, glm::vec3 scale, glm::vec3 hitboxPos = glm::vec3(0.0f), 
		glm::vec3 offsetFromspwan = glm::vec3(0.0f), int arrowDamage = 1, bool isPiercing = false, float lifeTime = 70.0f);

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
	std::array<Frosty::ECS::CTag*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Tag;
	//
	//std::array<Frosty::ECS::CArrow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Arrow;

	//temp? 
	bool m_OnlyOneSub = true;
};

#endif // !COMBATSYSTEM_HPP
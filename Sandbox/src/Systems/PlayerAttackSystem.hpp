//#ifndef PLAYER_ATTACK_SYSTEM_HPP
//#define PLAYER_ATTACK_SYSTEM_HPP
//#include "Frosty.h"
//#include "Frosty/API/AssetManager.hpp"
//
////will change later !
//class PlayerAttackSystem : public Frosty::ECS::BaseSystem
//{
//public:
//	PlayerAttackSystem() = default;
//	virtual ~PlayerAttackSystem() = default;
//
//	virtual void Init() override;
//
//	virtual void OnInput() override;
//
//	virtual void OnUpdate() override;
//
//	virtual void Render() override;
//
//	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//
//	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//
//private:
//	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
//	std::array<Frosty::ECS::CCollision*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Collision;
//	std::array<Frosty::ECS::CPlayerAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_PlayerAttack;
//	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
//	bool m_CanAttackNormal = true;
//	bool m_CanAttackArea = true;
//	bool m_CanAttackStrong = true;
//	bool m_CanSwitchWepond = false;
//};
//
//#endif // !PLAYER_ATTACK_SYSTEM_HPP
#include"Frosty/API/AssetManager/AM.hpp"

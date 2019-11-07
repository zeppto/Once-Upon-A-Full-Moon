#ifndef PLAYER_CONTROLLER_SYSTEM_HPP
#define PLAYER_CONTROLLER_SYSTEM_HPP

namespace MCS
{
	class PlayerControllerSystem : public Frosty::ECS::BaseSystem
	{
	public:
		PlayerControllerSystem() = default;
		virtual ~PlayerControllerSystem() = default;

		virtual void Init() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	private:
		glm::vec3 ScreenToTerrainPoint();
		void LookAtPoint(const glm::vec3& point, size_t index);
		void HandleMovement(size_t index);
		void HandleAttack(const glm::vec3& point, size_t index);
		void HandleInventory(size_t index);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPlayer*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Player;
		std::array<Frosty::ECS::CDash*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Dash;
		std::array<Frosty::ECS::CWeapon*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Weapon;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
		std::array<Frosty::ECS::CInventory*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Inventory;
	};
}

#endif // !PLAYER_CONTROLLER_SYSTEM_HPP
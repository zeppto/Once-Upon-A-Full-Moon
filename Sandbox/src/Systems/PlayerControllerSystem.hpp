#ifndef PLAYER_CONTROLLER_SYSTEM_HPP
#define PLAYER_CONTROLLER_SYSTEM_HPP

namespace Frosty { class PickUpEvent; }

namespace MCS
{
	class PlayerControllerSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		PlayerControllerSystem() = default;
		virtual ~PlayerControllerSystem() = default;

		virtual void Init() override;
		virtual void OnInput() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		glm::vec3 ScreenToTerrainPoint();
		void LookAtPoint(const glm::vec3& point, size_t index);
		void HandleMovement(size_t index);
		void HandleAttack(const glm::vec3& point, size_t index);
		void HandleInventory(size_t index);

		void OnPickUpEvent(Frosty::PickUpEvent& e);

		void CreateBoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);
		void CreateProjectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);

		void SwapWeapon(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		void SwapMesh(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		void SwapMaterial(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		float GenerateCriticalHit(float criticalHit, float criticalHitChance);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPlayer*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Player;
		std::array<Frosty::ECS::CDash*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Dash;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
		std::array<Frosty::ECS::CInventory*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Inventory;

		Frosty::World* m_World{ nullptr };

		bool m_keyPressed{ false };
	};
}

#endif // !PLAYER_CONTROLLER_SYSTEM_HPP

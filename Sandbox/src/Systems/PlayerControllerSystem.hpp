#ifndef PLAYER_CONTROLLER_SYSTEM_HPP
#define PLAYER_CONTROLLER_SYSTEM_HPP

namespace Frosty { class PickUpEvent; class UpgradeWeaponEvent; class HealAbilityEvent; class EnemyDeathEvent; }

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
		void HandleAttack(size_t index);
		void LVL1Attack(const size_t index);
		void LVL2Attack(const size_t index);
		void LVL3Attack(const size_t index);

		void CreateLVL1BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);
		void CreateLVL2BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);
		void CreateLVL3BoundingBox(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);

		void CreateLVL1Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);
		void CreateLVL2Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);
		void CreateLVL3Projectile(const std::shared_ptr<Frosty::ECS::Entity>& weaponCarrier, const std::shared_ptr<Frosty::ECS::Entity>& weapon);

		void CreateFireEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation);
		void CreateEarthEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation);
		void CreateWindEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation);
		void CreateWaterEffect(Frosty::ECS::CAttack& attack, glm::vec3 spawnPos, glm::vec3 rotation);

		float GenerateCriticalHit(float criticalHit, float criticalHitChance);

		void HandleInventory(size_t index);
		void OnPickUpEvent(Frosty::PickUpEvent& e);
		void OnUpgradeWeaponEvent();
		void OnHealAbilityEvent();
		void OnEnemyDeathEvent(Frosty::EnemyDeathEvent& e);

		void SwapWeapon(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		void SwapMesh(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		void SwapMaterial(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);
		void SwapLootType(const std::shared_ptr<Frosty::ECS::Entity>& playerWeapon, const std::shared_ptr<Frosty::ECS::Entity>& lootWeapon);

		void UpdateHUD(size_t index);
		void SetPickUpText(size_t index, std::string text);
		void ResetAllHUDWeaponInfo(size_t index);
		void OnDamage();
		void ScaleHudToScreen(size_t index);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPlayer*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Player;
		std::array<Frosty::ECS::CDash*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Dash;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
		std::array<Frosty::ECS::CInventory*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Inventory;

		Frosty::World* m_World{ nullptr };

		bool m_keyPressed{ false };

		float m_HeightMultiplier{ 0.0f };
		float m_WidthMultiplier{ 0.0f };
		float m_LastHeightMultiplier{ 0.0f };
		float m_LastWidthtMultiplier{ 0.0f };

		float m_WindowWidth = { 0.0f };
		float m_WindowHeight = { 0.0f };

	};
}
#endif // !PLAYER_CONTROLLER_SYSTEM_HPP
#ifndef INVENTORY_SYSTEM_HPP
#define INVENTORY_SYSTEM_HPP

namespace MCS
{
	class InventorySystem : public Frosty::ECS::BaseSystem
	{
	public:
		InventorySystem() = default;
		virtual ~InventorySystem() = default;

		virtual void Init() override;

		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	private:
		void OnEvent(Frosty::BaseEvent& e);
		void InventorySystem::OnKeyPressedEvent(Frosty::KeyPressedEvent& e);

	private:
		std::array<Frosty::ECS::CInventory*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Inventory;
		std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

	};
}
#endif // !CONSUMABLES_SYSTEM_HPP
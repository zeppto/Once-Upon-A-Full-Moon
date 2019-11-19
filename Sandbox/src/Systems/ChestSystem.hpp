#ifndef CHEST_SYSTEM_HPP
#define CHEST_SYSTEM_HPP

namespace MCS
{
	class ChestSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;
	public:
		ChestSystem() = default;
		virtual ~ChestSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;
	private:
		void SpawnItem(size_t index);
	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CChest*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Chest;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;

		std::shared_ptr<Frosty::ECS::Entity> m_Entity = nullptr;

		float m_CoolDown = 0.0f;
		unsigned int m_RandItem = 0;
	};
}
#endif

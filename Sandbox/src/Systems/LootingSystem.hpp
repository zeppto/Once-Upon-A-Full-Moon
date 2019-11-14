#ifndef LOOT_SYSTEM_HPP
#define LOOT_SYSTEM_HPP

namespace Frosty { class PickUpAttemptEvent; }

namespace MCS
{
	class LootingSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;
	public:
		LootingSystem() = default;
		virtual ~LootingSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;
	private:
		void OnPickUpAttemptEvent(Frosty::PickUpAttemptEvent& e);
		float Distance2D(glm::vec3 pos1, glm::vec3 pos2);
	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CLootable*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Lootable;
		std::array<std::shared_ptr<Frosty::ECS::Entity>, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Items;

		std::shared_ptr<Frosty::ECS::Entity> m_Player = nullptr;

		float m_WorkingArea = 3.0f;
		float m_CoolDown = 0.0f;
	};
}
#endif

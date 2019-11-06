#ifndef BOSS_BEHAVIOR_SYSTEM_HPP
#define BOSS_BEHAVIOR_SYSTEM_HPP

namespace Frosty { class BaitPlacedEvent; }

namespace MCS
{
	class BossBehaviorSystem : public Frosty::ECS::BaseSystem
	{
	public:
		BossBehaviorSystem() = default;
		virtual ~BossBehaviorSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	private:
		void OnBaitPlacedEvent(Frosty::BaitPlacedEvent& e);
	

	private:
		float CalcDistance2D(glm::vec3 pos1, glm::vec3 pos2);

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CBoss*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Boss;
		std::array<Frosty::ECS::CFollow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Follow;

	};
}

#endif // !BOSS_BEHAVIOR_SYSTEM_HPP

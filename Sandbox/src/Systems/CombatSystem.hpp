#ifndef COMBATSYSTEM_HPP
#define COMBATSYSTEM_HPP

namespace Frosty { class CollisionEvent; }

namespace MCS
{
	class CombatSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		CombatSystem() = default;
		virtual ~CombatSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void OnCollisionEvent(Frosty::CollisionEvent& e);

	private:
		Frosty::World* m_World{ nullptr };
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

	};
}
#endif // !COMBATSYSTEM_HPP
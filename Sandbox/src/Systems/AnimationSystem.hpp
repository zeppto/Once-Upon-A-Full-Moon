#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

namespace Frosty { class BasicAttackEvent; 
				   class DashEvent;
				 }

namespace MCS
{
	class AnimationSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		AnimationSystem() = default;
		virtual ~AnimationSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void OnBasicAttackEvent(Frosty::BasicAttackEvent& e);
		void OnDashEvent(Frosty::DashEvent& e);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CAnimController*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_AControllers;
		std::array<Frosty::ECS::CDash*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Dash;

	};
}
#endif // !ANIMATIONSYSTEM_HPP

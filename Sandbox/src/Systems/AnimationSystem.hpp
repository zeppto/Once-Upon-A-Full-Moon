#ifndef ANIMATIONSYSTEM_HPP
#define ANIMATIONSYSTEM_HPP

namespace Frosty { class BasicAttackEvent; 
				   class DashEvent;
				   class PlayAnimEvent;
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
		virtual void Render() override;
		virtual std::string GetInfo() const override;

	private:
		void OnBasicAttackEvent(Frosty::BasicAttackEvent& e);
		void OnDashEvent(Frosty::DashEvent& e);
		void UpdateAnimOffset(Frosty::ECS::CAnimController* ctrl);
		void OnPlayAnimEvent(Frosty::PlayAnimEvent& e);
		void BeginNewAnim(Frosty::ECS::CAnimController* controller, const std::string& animName);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CAnimController*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_AControllers;
		Frosty::World* m_World{ nullptr };

	};
}
#endif // !ANIMATIONSYSTEM_HPP

#ifndef FOLLOW_SYSTEM_HPP
#define FOLLOW_SYSTEM_HPP

namespace MCS
{
	class FollowSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		FollowSystem() = default;
		virtual ~FollowSystem() = default;

		virtual void Init() override;

		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CFollow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Follow;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
	};
}

#endif // !FOLLOW_SYSTEM_HPP

#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP


namespace MCS
{
	class PhysicsSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		PhysicsSystem() = default;
		virtual ~PhysicsSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void CheckCollision(size_t index);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;

		Frosty::World* m_World{ nullptr };

	};
}

#endif // !PHYSICS_SYSTEM_HPP

#ifndef SWORD_SYSTEM_HPP
#define SWORD_SYSTEM_HPP


namespace MCS
{
	class SwordSystem : public Frosty::ECS::BaseSystem
	{
	public:
		SwordSystem() = default;
		virtual ~SwordSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CSword*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Sword;

	};
}

#endif // !SWORD_SYSTEM_HPP

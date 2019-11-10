#ifndef ARROW_SYSTEM_HPP
#define ARROW_SYSTEM_HPP


namespace MCS
{
	class ArrowSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		ArrowSystem() = default;
		virtual ~ArrowSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CArrow*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Arrow;

	};
}

#endif // !ARROW_SYSTEM_HPP

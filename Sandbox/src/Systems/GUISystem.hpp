#ifndef GUI_SYSTEM_HPP
#define GUI_SYSTEM_HPP

namespace MCS
{
	class GUISystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		GUISystem() = default;
		virtual ~GUISystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void Render() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CGUI*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_GUI;
	};
}

#endif // !GUI_SYSTEM_HPP

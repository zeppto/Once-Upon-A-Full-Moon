#ifndef LIGHT_SYSTEM_HPP
#define LIGHT_SYSTEM_HPP

namespace MCS
{
	class LightSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		LightSystem() = default;
		virtual ~LightSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void Render() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CLight*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Light;
	};
}
#endif // !LIGHT_SYSTEM_HPP

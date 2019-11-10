#ifndef DESTROY_SYSTEM_HPP
#define DESTROY_SYSTEM_HPP

namespace Frosty { class DestroyEvent; }

namespace MCS
{
	class DestroySystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		DestroySystem() = default;
		virtual ~DestroySystem() = default;

		virtual void Init() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CDestroy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Destroy;

	};
}

#endif // !DESTROY_SYSTEM_HPP

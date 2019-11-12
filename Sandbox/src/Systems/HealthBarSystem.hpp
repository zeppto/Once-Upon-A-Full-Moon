#ifndef HEALTH_BAR_SYSTEM_HPP
#define HEALTH_BAR_SYSTEM_HPP
#include "Frosty.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	class HealthBarSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		HealthBarSystem() = default;
		virtual ~HealthBarSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void Render() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;
		std::array<Frosty::ECS::CHealthBar*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_HealthBar;

	};
}
#endif // !HEALTH_BAR_SYSTEM_HPP

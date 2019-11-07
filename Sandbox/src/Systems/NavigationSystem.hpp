#ifndef NAVIGATION_SYSTEM_HPP
#define NAVIGATION_SYSTEM_HPP

// TEMPORARY UNTIL COLLISION MAP
// OR MAKE A NEW (BETTER) ONE IN SCENE
#include "Map/GridMap.hpp"

namespace MCS
{
	class NavigationSystem : public Frosty::ECS::BaseSystem
	{
	public:
		NavigationSystem() = default;
		virtual ~NavigationSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

		void InitiateGridMap(const Frosty::ECS::CTransform& transform);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;

		std::unique_ptr<GridMap> m_GridMap;

	};
}

#endif // !NAVIGATION_SYSTEM_HPP
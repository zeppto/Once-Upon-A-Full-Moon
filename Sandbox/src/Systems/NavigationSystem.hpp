#ifndef NAVIGATION_SYSTEM_HPP
#define NAVIGATION_SYSTEM_HPP

// TEMPORARY UNTIL COLLISION MAP
// OR MAKE A NEW (BETTER) ONE IN SCENE
//#include "Map/GridMap.hpp"
//#include "Pathfinding/Grid.hpp"
#include "Pathfinding/Pathfinding.hpp"

namespace MCS
{
	class NavigationSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		NavigationSystem() = default;
		virtual ~NavigationSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

		void InitiateGridMap(const Frosty::ECS::CTransform& mapTransform);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;

		//std::unique_ptr<GridMap> m_GridMap;
		std::unique_ptr<Grid> m_Grid;
		std::unique_ptr<Pathfinding> m_Pathfinding;

	};
}

#endif // !NAVIGATION_SYSTEM_HPP

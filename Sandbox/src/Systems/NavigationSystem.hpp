#ifndef NAVIGATION_SYSTEM_HPP
#define NAVIGATION_SYSTEM_HPP

// TEMPORARY UNTIL COLLISION MAP
// OR MAKE A NEW (BETTER) ONE IN SCENE
//#include "Map/GridMap.hpp"
//#include "Pathfinding/Grid.hpp"
#include "Pathfinding/Pathfinding.hpp"

namespace Frosty { class InitiateGridEvent; }
namespace Frosty { class UpdateCurrentRoomEvent; }

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
		void OnEvent(Frosty::BaseEvent& e);

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

		void InitiateGridMap(const Frosty::ECS::CTransform& planeTransform);

	private:
		void OnInitiateGridMap(Frosty::InitiateGridEvent& e);
		void OnUpdateCurrentRoomEvent(Frosty::UpdateCurrentRoomEvent& e);
		void LookAtPoint(const glm::vec3& point, size_t index);

		void HandlePathfinding(size_t index);
		void HandleDistance(size_t index);
		void HandleEscape(size_t index);
		void HandleReset(size_t index);

	private:
		Frosty::World* m_World{ nullptr };

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;

		//std::unique_ptr<GridMap> m_GridMap;
		std::unique_ptr<Frosty::Grid> m_Grid;
		std::unique_ptr<Pathfinding> m_Pathfinding;
		std::shared_ptr<Frosty::Grid> m_CurrentActiveGridMap;

	};
}

#endif // !NAVIGATION_SYSTEM_HPP

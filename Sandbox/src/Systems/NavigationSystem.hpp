#ifndef NAVIGATION_SYSTEM_HPP
#define NAVIGATION_SYSTEM_HPP

// TEMPORARY UNTIL COLLISION MAP
// OR MAKE A NEW (BETTER) ONE IN SCENE
//#include "Map/GridMap.hpp"
//#include "Pathfinding/Grid.hpp"
#include "Pathfinding/Pathfinding.hpp"

namespace Frosty { class InitiateGridEvent; }
namespace Frosty { class UpdateCurrentRoomEvent; }
namespace Frosty { class SwitchRoomEvent; }

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
		void OnSwitchRoomEvent(Frosty::SwitchRoomEvent& e);
		void LookAtPoint(const glm::vec3& point, size_t index);

		void HandlePathfinding(size_t index, const uint32_t& EntityGroupID);
		void HandleDistance(size_t index);
		void HandleEscape(size_t index);
<<<<<<< HEAD
		void HandleReset(size_t index, const uint32_t& EntityGroupID);
=======
		void HandleReset(size_t index);
		void HandleDeath(size_t index);
>>>>>>> 0e1c2caaa9a95857c133dafa4e8671c7b7a57418

	private:
		Frosty::World* m_World{ nullptr };

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;
		std::array<Frosty::ECS::CEnemy*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Enemy;

		//std::unique_ptr<GridMap> m_GridMap;
		std::unique_ptr<Grid> m_Grid;
		std::unique_ptr<Pathfinding> m_Pathfinding;

		std::shared_ptr<Frosty::Grid> m_CurrentActiveGridMap {nullptr};
		std::shared_ptr<Pathfinding> m_CurrentActivePathfinding {nullptr};
		uint16_t m_GroupUpdate;


		struct GridMap
		{
			std::shared_ptr<Frosty::Grid> Grid{ nullptr };
			std::shared_ptr<Pathfinding> PathFinder{ nullptr };
			uint16_t EntityGroupID{ (uint16_t)-1 };

			GridMap& operator=(const GridMap& other)
			{
				if (&other != this)
				{
					Grid = other.Grid;
					PathFinder = other.PathFinder;
					EntityGroupID = other.EntityGroupID;
				}
				return *this;
			}
		};

		GridMap m_ActiveMap;
		GridMap m_OtherMap;
		
		std::shared_ptr<Frosty::Grid> m_SecondGridMap {nullptr};
		std::shared_ptr<Pathfinding> m_SecondPathfinding {nullptr};

	};
}

#endif // !NAVIGATION_SYSTEM_HPP

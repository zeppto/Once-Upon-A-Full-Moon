#ifndef LEVEL_SYSTEM_HPP
#define LEVEL_SYSTEM_HPP
#include "LevelHandeler/MapGenerator.hpp"
#include "LevelHandeler/LevelsHardCoded.hpp"
#include "LevelHandeler/LevelFileFormat.hpp"

namespace Frosty { class BoolMap; }
namespace Frosty { class ExitLevelEvent; }
namespace Frosty { class SaveLevelEvent; }
namespace Frosty { class OpenLevelEvent; }
namespace Frosty { class CreateLevelEvent; }
namespace Frosty { class CreatEntityEvent; }
namespace Frosty { class UpdatePlayerRoomCoordEvent; }
namespace MCS
{
	class LevelSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void OnExitLevelEvent(Frosty::ExitLevelEvent& e);
		void OnSaveLevelEvent(Frosty::SaveLevelEvent& e);
		void OnCreateLevelEvent(Frosty::CreateLevelEvent& e);
		void OnOpenLevelEvent(Frosty::OpenLevelEvent& e);
		void OnCreatEntityEvent(Frosty::CreatEntityEvent& e);
		void OnPlayerUpdateCoordEvent(Frosty::UpdatePlayerRoomCoordEvent& e);


	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;

		MapGenerator m_Map;
		
		bool m_CurrentRoomBool = true;
		std::string m_FirstRoom{ "" }; //False
		std::string m_SecondRoom{ "" }; //True

		glm::ivec2 m_PlayerCoords = { 10, 15 };//{ 10, 15 };
		glm::ivec2 m_OtherRoom = { -1, -1 };

		//map.generateMap();
		Room m_CurrentRoome;// = map.getRoom(glm::ivec2(11, 15));
		bool m_Start = true;

		//type of room (needed for creation of file)
		std::string m_RoomType = "unknown";
		LevelFileFormat m_LevelFileFormat;

		//temp
		bool m_NextLevel = false;
		int m_EntrensSide = -1;
		float m_TempTimer = 0;


		bool m_LoadMapBool = false;
		Frosty::ECS::CTransform* m_PlayerTransformLoadComponent;
		int m_LoadExitDir = -1;
		std::string m_LoadFileName = "";
		int m_MapRotation = 0;


		//int rotation = 0;
		//std::string texture = map.getRoomTextur(glm::ivec2(11, 15), &rotation);
		//Level::Room(myRoome.sideExits[0], myRoome.sideExits[1], myRoome.sideExits[2], myRoome.sideExits[3], texture, rotation);

	};
}


#endif

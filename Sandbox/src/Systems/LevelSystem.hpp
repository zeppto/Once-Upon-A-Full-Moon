#ifndef LEVEL_SYSTEM_HPP
#define LEVEL_SYSTEM_HPP
#include "LevelHandeler/MapGenerator.hpp"
#include "LevelHandeler/LevelsHardCoded.hpp"
#include "LevelHandeler/LevelFileFormat.hpp"

namespace Frosty { class ExitLevelEvent; }
namespace Frosty { class SaveLevelEvent; }
namespace Frosty { class CreateLevelEvent; }
namespace Frosty { class OpenLevelEvent; }
namespace Frosty { class CreatEntityEvent; }
namespace Frosty { class BoolMap; }
namespace Frosty { class ResetEvent; }
namespace Frosty { class BossSpawnedEvent; }
namespace Frosty { class BaitPlacedEvent; }

struct bossRememberdPath
{
	std::vector<glm::ivec2> pathToGo;
	glm::ivec2 expectedPlayerPos = { -1, -1 };
	int lastTile = 1;
};

namespace MCS
{
	class LevelSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		virtual void Init() override;
		virtual void OnStart() override;
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
		void OnResetEvent(Frosty::ResetEvent& e);
		Frosty::ECS::CGUI* GetPlayerGUI();

		void OnBossSpawnedEvent(Frosty::BossSpawnedEvent& e);
		void OnBaitPlacedEvent(Frosty::BaitPlacedEvent& e);

		void randomBossMovment();
	private:
		Frosty::World* m_World;

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;

		MapGenerator m_Map;
		glm::ivec2 m_PlayerPos = { 10, 15 };//{ 10, 15 };
		glm::ivec2 m_BossPos = { 9, 15 };
		glm::ivec2 m_BossLastRoom = { -1, -1 };
		float m_StartTimer = 0.0f;
		float m_BossTimer = 0.0f;
		//needs balensing m_BossRoomTimer 
		//float m_BossRoomTimer = 40.0f;
		float m_BossRoomTimer = 100.0f; //in sec (max)
		float m_BossFollowTimer = 2.0f; //in min

		float m_BossHawol = 0.0f;
		float m_BossStartTimer = 10.0f;
		bossRememberdPath m_BossRememberdPath;
		std::vector<glm::ivec2> m_RoomswhithBait;

		//temp bools
		bool m_haveStartedMoving = false;


		//map.generateMap();
		Room m_CurrentRoome;// = map.getRoom(glm::ivec2(11, 15));
		bool m_Start = true;

		//type of room (needed for creation of file)
		std::string m_RoomType = "unknown";
		LevelFileFormat m_LevelFileFormat;

		Frosty::ECS::CGUI* m_GUI = nullptr;

		//temp
		bool m_NextLevel = false;
		int m_EntrensSide = -1;
		float m_TempTimer = 0;
		Frosty::ECS::CTransform* m_PlayerTransform = nullptr;
		bool m_CreatNewRoom = false;
		bool m_LodeNamedRoom = false;
		bool m_BossSpawned{ false };

		//int rotation = 0;
		//std::string texture = map.getRoomTextur(glm::ivec2(11, 15), &rotation);
		//Level::Room(myRoome.sideExits[0], myRoome.sideExits[1], myRoome.sideExits[2], myRoome.sideExits[3], texture, rotation);
	};
}
#endif

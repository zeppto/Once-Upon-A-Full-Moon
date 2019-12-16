#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP
//#include "Frosty.h"

namespace MCS
{
	struct Room
	{
		bool Ocupide = false;
		//up = [0], down = [1], right = [2], left = [3]
		bool sideExits[4] = { false };
	};

	struct RoomOnTile
	{
		glm::ivec2 pos;
		int startSide;
	};

	struct pathOnTile
	{
		glm::ivec2 pos = { 0, 0 };
		int distensTo = -1;
		bool sideExits[4] = { false };
		std::vector<glm::ivec2> parantTile;
	};


	class MapGenerator
	{
	public:
		MapGenerator();
		~MapGenerator();
		void generateMap();
		void generateRoomHandeler(glm::ivec2 startPos, int startSide, int nrToGenerate);
		bool generateRoom(glm::ivec2 startPos, int startSide, bool notLastBatc);
		glm::ivec2 posOffset(int i);
		Room getRoom(glm::ivec2 pos);
		glm::ivec2 getLastCreatedLevelPos();
		std::string getRoomTextur(glm::ivec2 pos, int* rotation);

		//for boss
		std::vector<glm::ivec2> getPathToTargert(glm::ivec2 startPos, glm::ivec2 endPos);
		bool isTargetInNextRoom(glm::ivec2 bossPos, glm::ivec2 targetPos);

	private:
		Room m_TileMap[30][30];
		std::vector<RoomOnTile> m_LoosEndsRooms;
		glm::ivec2 m_LastCreatedRoom;
	};
}


#endif


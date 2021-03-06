#ifndef MAP_GENERATOR_HPP
#define MAP_GENERATOR_HPP
//#include "Frosty.h"

namespace MCS
{
	enum RoomType
	{
		UpDown,				//0
		LeftRight,			//1
		LeftUp,				//2
		RightUp,			//3
		LeftDown,			//4
		RightDown,			//5
		LeftRightUp,		//6
		LeftRightDown,		//7
		LeftUpDown,			//8
		RightUpDown,		//9
		LeftRightUpDown,	//10
		//dead ends
		Up,					//11
		Down,				//12
		Left,				//13
		Right,				//14
	};


	struct Room
	{
		bool Ocupide = false;
		//up = [0], down = [1], right = [2], left = [3]
		bool sideExits[4] = { false };
		//bool LeftExit = false;
		//bool RightExit = false;
		//bool UpExit = false;
		//bool DownExit = false;
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


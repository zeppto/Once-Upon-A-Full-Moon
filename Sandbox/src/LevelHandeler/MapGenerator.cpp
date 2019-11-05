#include <fypch.hpp>
#include <mcspch.hpp>
#include "MapGenerator.hpp"

MapGenerator::MapGenerator()
{
	//tileMap[30][30];
}

MapGenerator::~MapGenerator()
{
}

void MapGenerator::generateMap()
{
	glm::ivec2 startPos = glm::ivec2(0);

	// up, down, left, right
	bool sides[4] = {false};

	//start pos
	startPos = glm::ivec2(10, 15);
	tileMap[startPos.x][startPos.y].Ocupide = true;
	//bloked
	tileMap[startPos.x][startPos.y].sideExits[2] = false;
	tileMap[startPos.x][startPos.y].sideExits[3] = true;

	RoomOnTile start;
	start.pos = startPos;
	start.startSide = 2;
	loosEndsRooms.push_back(start);

	generateRoomHandeler(startPos, 2, 10);

	int stop;
}

void MapGenerator::generateRoomHandeler(glm::ivec2 startPos, int startSide, int nrToGenerate)
{
	for (int i = 0; i < nrToGenerate; i++)
	{ 
		if (loosEndsRooms.size() > 0)
		{
			generateRoom(loosEndsRooms.at(0).pos, loosEndsRooms.at(0).startSide);
			loosEndsRooms.erase(loosEndsRooms.begin());
		}
	}
	//nrToGenerate--;
	//if (nrToGenerate > 0)
	//{
	//	if (tileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x, startPos.y + 1), nrToGenerate);
	//	if (tileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x, startPos.y - 1), nrToGenerate);
	//	if (tileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x + 1, startPos.y), nrToGenerate);
	//	if (tileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x - 1, startPos.y), nrToGenerate);
	//}
}

void MapGenerator::generateRoom(glm::ivec2 startPos, int startSide)
{
	//create left path
	glm::ivec2 pos(0);
	//come from
	//up
	if (startSide == 0)
		pos = glm::ivec2(startPos.x, startPos.y  - 1);
	//down
	if (startSide == 1)
		pos = glm::ivec2(startPos.x, startPos.y + 1);
	//left
	if (startSide == 2)
		pos = glm::ivec2(startPos.x + 1, startPos.y);
	//right
	if (startSide == 3)
		pos = glm::ivec2(startPos.x - 1, startPos.y);


	int nrOfPotentialExits = 0;

	for (int i = 0; i < 4; i++)
	{
		if (!tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide)
		{
			int isDeadEnd = 0;
			for (int j = 0; j < 4; j++)
			{
				if (tileMap[pos.x + posOffset(i).x + posOffset(j).x][pos.y + posOffset(i).y + posOffset(j).y].Ocupide)
				{
					isDeadEnd++;
				}
			}
			if (isDeadEnd < 4)
			{
				tileMap[pos.x][pos.y].sideExits[i] = true;
				nrOfPotentialExits++;
			}
		}
	}

	if (nrOfPotentialExits == 0)
	{
		//error
	}
	else
	{
		int nrToRemove = rand() % (nrOfPotentialExits);
		int numberOfExits = nrOfPotentialExits - nrToRemove;
		while (nrOfPotentialExits != numberOfExits)
		{
			int remove = rand() % 4;
			if (tileMap[pos.x][pos.y].sideExits[remove])
			{
				tileMap[pos.x][pos.y].sideExits[remove] = false;
				nrOfPotentialExits--;
			}
		}

		
		for (int i = 0; i < 4; i++)
		{
			//loosEndsRooms.push_back(pos);
			//loosEndsRooms.erase(loosEndsRooms.begin());
			RoomOnTile temp;
			temp.pos = pos;
			if (tileMap[pos.x][pos.y].sideExits[i])
			{

				temp.startSide = i;
				loosEndsRooms.push_back(temp);
			}
		}

		tileMap[pos.x][pos.y].Ocupide = true;
		tileMap[pos.x][pos.y].sideExits[startSide] = true;

	}
}

glm::ivec2 MapGenerator::posOffset(int i)
{
	if (i == 0)
		return glm::ivec2( 0, -1);
	if (i == 1)
		return glm::ivec2( 0,  1);
	if (i == 2)
		return glm::ivec2( 1,  0);
	if (i == 3)
		return glm::ivec2(-1,  0);
}

Room MapGenerator::getRoom(glm::ivec2 pos)
{
	return tileMap[pos.x][pos.y];
}

std::string MapGenerator::getRoomTextur(glm::ivec2 pos, int* rotation)
{
	//enum RoomType
	//{
	//	UpDown,				//0
	//	LeftRight,			//1
	//	LeftUp,				//2
	//	RightUp,			//3
	//	LeftDown,			//4
	//	RightDown,			//5
	//	LeftRightUp,		//6
	//	LeftRightDown,		//7
	//	LeftUpDown,			//8
	//	RightUpDown,		//9
	//	LeftRightUpDown,	//10
	//	//dead ends
	//	Up,					//11
	//	Down,				//12
	//	Left,				//13
	//	Right,				//14
	//};

	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTempStraght";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempStraght";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTempBend";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempBend";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		return "RoomTempBend";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		return "RoomTempBend";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTemp3";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempCross";
	}
	return "red";
}

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
	start.startSide = 3;
	loosEndsRooms.push_back(start);

	generateRoomHandeler(startPos, 3, 5);
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
		pos = glm::ivec2(startPos.x - 1, startPos.y);
	//right
	if (startSide == 3)
		pos = glm::ivec2(startPos.x + 1, startPos.y);


	int nrOfPotentialExits = 0;

	for (int i = 0; i < 4; i++)
	{
		if (!tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide)
		{
			tileMap[pos.x][pos.y].sideExits[i] = true;
			nrOfPotentialExits++;
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
		return glm::ivec2( 0,  1);
	if (i == 1)
		return glm::ivec2( 0, -1);
	if (i == 2)
		return glm::ivec2( 1,  0);
	if (i == 3)
		return glm::ivec2(-1,  0);
}

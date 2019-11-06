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

	//start pos
	startPos = glm::ivec2(10, 15);
	tileMap[startPos.x][startPos.y].Ocupide = true;
	//bloked
	tileMap[startPos.x][startPos.y].sideExits[2] = false;
	tileMap[startPos.x][startPos.y].sideExits[3] = true;

	//startPos = glm::ivec2(10, 15);
	//tileMap[startPos.x][startPos.y].Ocupide = true;
	////bloked
	//tileMap[startPos.x][startPos.y].sideExits[0] = true;
	//tileMap[startPos.x][startPos.y].sideExits[1] = false;
	//tileMap[startPos.x][startPos.y].sideExits[2] = true;
	//tileMap[startPos.x][startPos.y].sideExits[3] = true;


	RoomOnTile start;
	start.pos = startPos;
	start.startSide = 3;
	loosEndsRooms.push_back(start);

	generateRoomHandeler(startPos, 3, 300);
}

void MapGenerator::generateRoomHandeler(glm::ivec2 startPos, int startSide, int nrToGenerate)
{
	for (int i = 0; i < nrToGenerate;)
	{ 
		if (loosEndsRooms.size() > 0)
		{
			if (generateRoom(loosEndsRooms.at(0).pos, loosEndsRooms.at(0).startSide, nrToGenerate - i > loosEndsRooms.size()))
				i++;
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

bool MapGenerator::generateRoom(glm::ivec2 startPos, int startSide, bool notLastBatch)
{
	//create left path
	glm::ivec2 pos(0);
	//come from
	pos = startPos + posOffset(startSide);

	//temp
	if (pos.x == 12 && pos.y == 15)
		pos.x = 12;

	int nrOfPotentialExits = 0;

	if (!tileMap[pos.x][pos.y].Ocupide)
	{
		for (int i = 0; i < 4; i++)
		{
			tileMap[pos.x][pos.y].sideExits[i] = false;
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
		int numberOfExits = 0;
		if (nrOfPotentialExits != 0)
		{
			int nrToRemove = rand() % (nrOfPotentialExits);
			numberOfExits = nrOfPotentialExits - nrToRemove;
			while (nrOfPotentialExits != numberOfExits)
			{
				int remove = rand() % 4;
				if (tileMap[pos.x][pos.y].sideExits[remove])
				{
					tileMap[pos.x][pos.y].sideExits[remove] = false;
					nrOfPotentialExits--;
				}
			}

			if (notLastBatch)
			{
				for (int i = 0; i < 4; i++)
				{
					//loosEndsRooms.push_back(pos);
					//loosEndsRooms.erase(loosEndsRooms.begin());
					RoomOnTile temp;
					temp.pos = pos;
					if (tileMap[pos.x][pos.y].sideExits[i] && !tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide)
					{
						temp.startSide = i;
						loosEndsRooms.push_back(temp);
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
					tileMap[pos.x][pos.y].sideExits[i] = false;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide && startSide != (1 + (i / 2) * 4 - i))
			{
				if (tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[1 + (i / 2)*4 - i])
				{
					tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[1 + (i / 2) * 4 - i] = false;
					
					int nrOfCloseRoomExits = 0;
					for (int k = 0; k < 4; k++)
					{
						if (tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[k])
							nrOfCloseRoomExits++;
					}
					if (nrOfCloseRoomExits < 2 && pos + posOffset(i) != glm::ivec2(10, 15))
					{
						int nrToAdd = 2 + nrOfCloseRoomExits;
						tileMap[pos.x][pos.y].sideExits[0];
						while (nrToAdd != nrOfCloseRoomExits)
						{
							int add = rand() % 4;
							if (!tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] && pos + posOffset(i) + posOffset(add) != glm::ivec2(10, 15))
							{
								tileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] = true;
								tileMap[pos.x + posOffset(i).x + posOffset(add).x][pos.y + posOffset(i).y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
								nrOfCloseRoomExits++;
							}
						}
					}
				}
			}
		}


		tileMap[pos.x][pos.y].Ocupide = true;
		tileMap[pos.x][pos.y].sideExits[1 + (startSide / 2) * 4 - startSide] = true;
		numberOfExits = 0;

		for (int i = 0; i < 4; i++)
		{
			if(tileMap[pos.x][pos.y].sideExits[i])
				numberOfExits++;
		}

		//fix Deadends
		if (numberOfExits < 2)
		{
			int nrOfEntrensesInParantRoom = 0;
			for (int i = 0; i < 4; i++)
			{
				if (tileMap[startPos.x][startPos.y].sideExits[i])
					nrOfEntrensesInParantRoom++;
			}
			if (nrOfEntrensesInParantRoom < 3 && pos != glm::ivec2(10, 15))
			{
				int nrToAdd = 2 + numberOfExits;
				while (nrToAdd != numberOfExits)
				{
					int add = rand() % 4;
					if (!tileMap[pos.x][pos.y].sideExits[add] && pos + posOffset(add) != glm::ivec2(10, 15))
					{
						tileMap[pos.x + posOffset(add).x][pos.y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
						tileMap[pos.x][pos.y].sideExits[add] = true;
						numberOfExits++;
					}
				}

			}
		}
		return true;
	}
	return false;
}

glm::ivec2 MapGenerator::posOffset(int i)
{
	if (i == 0)
		return glm::ivec2( 0, -1);
	if (i == 1)
		return glm::ivec2( 0,  1);
	if (i == 2)
		return glm::ivec2(-1,  0);
	if (i == 3)
		return glm::ivec2( 1,  0);
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
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTempBend";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempBend";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		return "RoomTempBend";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		return "RoomTempBend";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTemp3";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		return "RoomTemp3";
	}
	if (tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempCross";
	}
	//dead ends
	if (tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		return "RoomTempDeadEnd";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		return "RoomTempDeadEnd";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && !tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		return "RoomTempDeadEnd";
	}
	if (!tileMap[pos.x][pos.y].sideExits[0] && !tileMap[pos.x][pos.y].sideExits[1] && !tileMap[pos.x][pos.y].sideExits[2] && tileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		return "RoomTempDeadEnd";
	}
	if (tileMap[pos.x][pos.y].Ocupide)
		return "HeartFull";
	return "red";
}

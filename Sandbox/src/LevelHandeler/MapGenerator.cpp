#include <mcspch.hpp>
#include "MapGenerator.hpp"

MapGenerator::MapGenerator()
{
	//m_m_TileMap[30][30];
}

MapGenerator::~MapGenerator()
{
}

void MapGenerator::generateMap()
{
	glm::ivec2 startPos = glm::ivec2(0);

	//start pos
	startPos = glm::ivec2(10, 15);
	m_TileMap[startPos.x][startPos.y].Ocupide = true;
	//bloked
	m_TileMap[startPos.x][startPos.y].sideExits[2] = false;
	m_TileMap[startPos.x][startPos.y].sideExits[3] = true;

	//startPos = glm::ivec2(10, 15);
	//m_TileMap[startPos.x][startPos.y].Ocupide = true;
	////bloked
	//m_TileMap[startPos.x][startPos.y].sideExits[0] = true;
	//m_TileMap[startPos.x][startPos.y].sideExits[1] = false;
	//m_TileMap[startPos.x][startPos.y].sideExits[2] = true;
	//m_TileMap[startPos.x][startPos.y].sideExits[3] = true;


	RoomOnTile start;
	start.pos = startPos;
	start.startSide = 3;
	m_LoosEndsRooms.push_back(start);

	generateRoomHandeler(startPos, 3, 300);
}

void MapGenerator::generateRoomHandeler(glm::ivec2 startPos, int startSide, int nrToGenerate)
{
	for (int i = 0; i < nrToGenerate;)
	{ 
		if (m_LoosEndsRooms.size() > 0)
		{
			if (generateRoom(m_LoosEndsRooms.at(0).pos, m_LoosEndsRooms.at(0).startSide, nrToGenerate - i > m_LoosEndsRooms.size()))
				i++;
			m_LoosEndsRooms.erase(m_LoosEndsRooms.begin());
		}
	}
	//nrToGenerate--;
	//if (nrToGenerate > 0)
	//{
	//	if (m_TileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x, startPos.y + 1), nrToGenerate);
	//	if (m_TileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x, startPos.y - 1), nrToGenerate);
	//	if (m_TileMap[startPos.x][startPos.y].sideExits[0])
	//		generateRoomHandeler(glm::ivec2(startPos.x + 1, startPos.y), nrToGenerate);
	//	if (m_TileMap[startPos.x][startPos.y].sideExits[0])
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

	if (!m_TileMap[pos.x][pos.y].Ocupide)
	{
		for (int i = 0; i < 4; i++)
		{
			m_TileMap[pos.x][pos.y].sideExits[i] = false;
			if (!m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide)
			{
				int isDeadEnd = 0;
				for (int j = 0; j < 4; j++)
				{
					if (m_TileMap[pos.x + posOffset(i).x + posOffset(j).x][pos.y + posOffset(i).y + posOffset(j).y].Ocupide)
					{
						isDeadEnd++;
					}
				}
				if (isDeadEnd < 4)
				{
					m_TileMap[pos.x][pos.y].sideExits[i] = true;
					nrOfPotentialExits++;
				}
			}
		}
		int numberOfExits = 0;
		if (nrOfPotentialExits != 0)
		{
			int nrToRemove = rand() % (nrOfPotentialExits);
			numberOfExits = nrOfPotentialExits - nrToRemove;
			while (nrOfPotentialExits != numberOfExits && m_SaftyNr < 20)
			{
				int remove = rand() % 4;
				if (m_TileMap[pos.x][pos.y].sideExits[remove])
				{
					m_TileMap[pos.x][pos.y].sideExits[remove] = false;
					nrOfPotentialExits--;
				}
				m_SaftyNr++;
			}
			m_SaftyNr = 0;

			if (notLastBatch)
			{
				for (int i = 0; i < 4; i++)
				{
					//m_LoosEndsRooms.push_back(pos);
					//m_LoosEndsRooms.erase(m_LoosEndsRooms.begin());
					RoomOnTile temp;
					temp.pos = pos;
					if (m_TileMap[pos.x][pos.y].sideExits[i] && !m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide)
					{
						temp.startSide = i;
						m_LoosEndsRooms.push_back(temp);
					}
				}
			}
			else
			{
				for (int i = 0; i < 4; i++)
					m_TileMap[pos.x][pos.y].sideExits[i] = false;
			}
		}

		for (int i = 0; i < 4; i++)
		{
			if (m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].Ocupide && startSide != (1 + (i / 2) * 4 - i))
			{
				if (m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[1 + (i / 2)*4 - i])
				{
					m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[1 + (i / 2) * 4 - i] = false;
					
					int nrOfCloseRoomExits = 0;
					for (int k = 0; k < 4; k++)
					{
						if (m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[k])
							nrOfCloseRoomExits++;
					}
					if (nrOfCloseRoomExits < 2 && pos + posOffset(i) != glm::ivec2(10, 15))
					{
						int nrToAdd = 2 + nrOfCloseRoomExits;
						m_TileMap[pos.x][pos.y].sideExits[0];
						while (nrToAdd != nrOfCloseRoomExits && m_SaftyNr < 20)
						{
							int add = rand() % 4;
							if (!m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] && pos + posOffset(i) + posOffset(add) != glm::ivec2(10, 15))
							{
								m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] = true;
								m_TileMap[pos.x + posOffset(i).x + posOffset(add).x][pos.y + posOffset(i).y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
								nrOfCloseRoomExits++;
							}
							m_SaftyNr++;
						}
						m_SaftyNr = 0;
					}
				}
			}
		}


		m_TileMap[pos.x][pos.y].Ocupide = true;
		m_TileMap[pos.x][pos.y].sideExits[1 + (startSide / 2) * 4 - startSide] = true;
		numberOfExits = 0;

		for (int i = 0; i < 4; i++)
		{
			if(m_TileMap[pos.x][pos.y].sideExits[i])
				numberOfExits++;
		}

		//fix Deadends
		if (numberOfExits < 2)
		{
			int nrOfEntrensesInParantRoom = 0;
			for (int i = 0; i < 4; i++)
			{
				if (m_TileMap[startPos.x][startPos.y].sideExits[i])
					nrOfEntrensesInParantRoom++;
			}
			if (nrOfEntrensesInParantRoom < 3 && pos != glm::ivec2(10, 15))
			{
				int nrToAdd = 2 + numberOfExits;
				while (nrToAdd != numberOfExits && m_SaftyNr < 20)
				{
					int add = rand() % 4;
					if (!m_TileMap[pos.x][pos.y].sideExits[add] && pos + posOffset(add) != glm::ivec2(10, 15))
					{
						m_TileMap[pos.x + posOffset(add).x][pos.y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
						m_TileMap[pos.x][pos.y].sideExits[add] = true;
						numberOfExits++;
					}
					m_SaftyNr++;
				}
				m_SaftyNr = 0;

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

	return glm::ivec2(0, 0);
}

Room MapGenerator::getRoom(glm::ivec2 pos)
{
	return m_TileMap[pos.x][pos.y];
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

	if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		//return "RoomTempStraght";
		return "straightRoad_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		//return "RoomTempStraght";
		return "straightRoad_chest_IsStaticks";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		//return "RoomTempBend";
		return "turningRoad_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		//return "RoomTempBend";
		return "turningRoad_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		//return "RoomTempBend";
		return "turningRoad_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		//return "RoomTempBend";
		return "turningRoad_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		//return "RoomTemp3";
		return "threeWayRoad_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		//return "RoomTemp3";
		return "threeWayRoad_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		//return "RoomTemp3";
		return "threeWayRoad_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		//return "RoomTemp3";
		return "threeWayRoad_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		//return "RoomTempCross";
		return "crossroad_chests_IsStatick";
	}
	//dead ends
	if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 0;
		//return "RoomTempDeadEnd";
		return "deadend_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 90;
		//return "RoomTempDeadEnd";
		return "deadend_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 180;
		//return "RoomTempDeadEnd";
		return "deadend_chests_IsStatick";
	}
	if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
	{
		*rotation = 270;
		//return "RoomTempDeadEnd";
		return "deadend_chests_IsStatick";
	}
	if (m_TileMap[pos.x][pos.y].Ocupide)
		return "deadend_chests_IsStatick";
	return "deadend_chests_IsStatick";
}

#include <mcspch.hpp>
#include "MapGenerator.hpp"

namespace MCS
{
	MapGenerator::MapGenerator()
	{
	}

	MapGenerator::~MapGenerator()
	{
	}

	void
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		






























































































































































































































































































































MapGenerator::generateMap()
	{
		glm::ivec2 startPos = glm::ivec2(0);
		
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				m_TileMap[i][j] = Room();
			}
		}

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
				if (generateRoom(m_LoosEndsRooms[0].pos, m_LoosEndsRooms[0].startSide, nrToGenerate - i > m_LoosEndsRooms.size()))
				{
					if (m_LoosEndsRooms.size() == 1 || i == nrToGenerate - 1)
					{
						m_LastCreatedRoom = m_LoosEndsRooms[0].pos;
					}
					i++;
				}
				m_LoosEndsRooms.erase(m_LoosEndsRooms.begin());
			}
			else
			{
				i++;
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

		if (pos.x < 30 && pos.y < 30 && pos.x > -1 && pos.y > -1)
		{
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
					while (nrOfPotentialExits != numberOfExits)
					{
						int remove = rand() % 4;
						if (m_TileMap[pos.x][pos.y].sideExits[remove])
						{
							m_TileMap[pos.x][pos.y].sideExits[remove] = false;
							nrOfPotentialExits--;
						}
					}

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
						if (m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[1 + (i / 2) * 4 - i])
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
								while (nrToAdd != nrOfCloseRoomExits)
								{
									int add = rand() % 4;
									if (!m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] && pos + posOffset(i) + posOffset(add) != glm::ivec2(10, 15))
									{
										m_TileMap[pos.x + posOffset(i).x][pos.y + posOffset(i).y].sideExits[add] = true;
										m_TileMap[pos.x + posOffset(i).x + posOffset(add).x][pos.y + posOffset(i).y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
										nrOfCloseRoomExits++;
									}
								}
							}
						}
					}
				}

				m_TileMap[pos.x][pos.y].Ocupide = true;
				m_TileMap[pos.x][pos.y].sideExits[1 + (startSide / 2) * 4 - startSide] = true;
				numberOfExits = 0;

				for (int i = 0; i < 4; i++)
				{
					if (m_TileMap[pos.x][pos.y].sideExits[i])
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
						while (nrToAdd != numberOfExits)
						{
							int add = rand() % 4;
							if (!m_TileMap[pos.x][pos.y].sideExits[add] && pos + posOffset(add) != glm::ivec2(10, 15))
							{
								m_TileMap[pos.x + posOffset(add).x][pos.y + posOffset(add).y].sideExits[1 + (add / 2) * 4 - add] = true;
								m_TileMap[pos.x][pos.y].sideExits[add] = true;
								numberOfExits++;
							}
						}
					}
				}
				return true;
			}
		}
		return false;
	}

	glm::ivec2 MapGenerator::posOffset(int i)
	{
		if (i == 0)
			return glm::ivec2(0, -1);
		if (i == 1)
			return glm::ivec2(0, 1);
		if (i == 2)
			return glm::ivec2(-1, 0);
		if (i == 3)
			return glm::ivec2(1, 0);

		return glm::ivec2(0, 0);
	}

	Room MapGenerator::getRoom(glm::ivec2 pos)
	{
		return m_TileMap[pos.x][pos.y];
	}

	glm::ivec2 MapGenerator::getLastCreatedLevelPos()
	{
		return m_LastCreatedRoom;
	}

	std::string MapGenerator::getRoomTextur(glm::ivec2 pos, int* rotation)
	{
		if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 90;
			//return "RoomTempStraght";
			return "straightRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 0;
			//return "RoomTempStraght";
			return "straightRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 90;
			//return "RoomTempBend";
			return "turningRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 0;
			//return "RoomTempBend";
			return "turningRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 180;
			//return "RoomTempBend";
			return "turningRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 270;
			//return "RoomTempBend";
			return "turningRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 0;
			//return "RoomTemp3";
			return "threeWayRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 90;
			//return "RoomTemp3";
			return "threeWayRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 180;
			//return "RoomTemp3";
			return "threeWayRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 270;
			//return "RoomTemp3";
			return "threeWayRoad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 0;
			//return "RoomTempCross";
			return "crossroad_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		//dead ends
		if (m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 0;
			//return "RoomTempDeadEnd";
			return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 90;
			//return "RoomTempDeadEnd";
			return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && !m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 180;
			//return "RoomTempDeadEnd";
			return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (!m_TileMap[pos.x][pos.y].sideExits[0] && !m_TileMap[pos.x][pos.y].sideExits[1] && !m_TileMap[pos.x][pos.y].sideExits[2] && m_TileMap[pos.x][pos.y].sideExits[3])
		{
			*rotation = 270;
			//return "RoomTempDeadEnd";
			return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
		}
		if (m_TileMap[pos.x][pos.y].Ocupide)
			return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
		return "deadend_chests_IsStatick_t_p_e_r_h_a_bb";
	}

	std::vector<glm::ivec2> MapGenerator::getPathToTargert(glm::ivec2 startPos, glm::ivec2 endPos)
	{
		pathOnTile pathRooms[30][30];
		std::vector<glm::ivec2> rooms;
		pathOnTile startRoom;
		startRoom.pos = startPos;
		startRoom.distensTo = 0;
		pathRooms[startRoom.pos.x][startRoom.pos.y].distensTo = startRoom.distensTo;
		for (int j = 0; j < 4; j++)
		{
			Room theStartRoom = getRoom(startRoom.pos);
			pathRooms[startRoom.pos.x][startRoom.pos.y].sideExits[j] = theStartRoom.sideExits[j];
		}
		std::deque<pathOnTile> roomsToGoThrough;
		roomsToGoThrough.emplace_back(startRoom);

		while (roomsToGoThrough.size() > 0)
		{
			pathOnTile room = roomsToGoThrough.front();
			Room currentRoom = getRoom(room.pos);
			for (int i = 0; i < 4; i++)
			{
				if (currentRoom.sideExits[i])
				{
					Room neighbor = getRoom(room.pos + posOffset(i));
					if (neighbor.Ocupide)
					{
						pathOnTile newPath;
						newPath.pos = room.pos + posOffset(i);
						newPath.distensTo = room.distensTo + 1;
						if (pathRooms[newPath.pos.x][newPath.pos.y].distensTo == -1)
						{
							pathRooms[newPath.pos.x][newPath.pos.y].distensTo = newPath.distensTo;
							for (int j = 0; j < 4; j++)
							{
								newPath.sideExits[j] = neighbor.sideExits[j];
								pathRooms[newPath.pos.x][newPath.pos.y].sideExits[j] = neighbor.sideExits[j];
							}
							newPath.parantTile = room.parantTile;
							newPath.parantTile.push_back(room.pos);

							roomsToGoThrough.emplace_back(newPath);

						}
						if (newPath.pos == endPos)
						{
							rooms = newPath.parantTile;
							rooms.push_back(newPath.pos);

							return rooms;
						}
					}
				}
			}
			roomsToGoThrough.pop_front();
		}
		return rooms;
	}
	bool MapGenerator::isTargetInNextRoom(glm::ivec2 bossPos, glm::ivec2 targetPos)
	{
		pathOnTile startRoom;
		startRoom.pos = bossPos;
		startRoom.distensTo = 0;
		pathOnTile room = startRoom;
		Room currentRoom = getRoom(room.pos);
		for (int i = 0; i < 4; i++)
		{
			if (currentRoom.sideExits[i])
			{
				pathOnTile newRoom;
				newRoom.pos = room.pos + posOffset(i);
				if (newRoom.pos == targetPos)
				{
					return true;
				}
			}
		}
		return false;
	}
}


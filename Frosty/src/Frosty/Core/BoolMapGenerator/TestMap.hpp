#ifndef  NODEMAP_HPP
#define NODEMAP_HPP

#include <sstream> 
#include<fstream>

namespace Frosty
{

#define MAXEXITS (uint8_t)4
#define DEADENDLENGTH (uint16_t)2
#define LocR (uint16_t)100
#define LocL (uint16_t)101
#define LocF (uint16_t)102
#define CngDir (uint16_t)3
#define PixUnit (uint16_t)10

	

	enum Direction{
		North = (int8_t)3,
		West = (int8_t)2,
		East = (int8_t)1,
		South = (int8_t)0
	};

	enum LocalDirection
	{
		Left = (int8_t)2,
		Right = (int8_t)1,
		Forward = (int8_t)0
	};

	const Direction STARTDIR = North;

	class Node
	{
	private:
		
		bool m_ConnectAble;
		uint16_t m_GroupID;

		uint16_t m_CoordX;
		uint16_t m_CoordY;

		bool m_Exits [MAXEXITS];

	public:
		inline Node(const Direction& Entrance,const uint16_t& GrpID,const uint16_t& CoordX, const uint16_t& CoordY, const bool& ConnectAble)
			: m_GroupID(GrpID), 
			m_ConnectAble(ConnectAble),
			m_CoordX(CoordX),
			m_CoordY(CoordY)
		{ 	for (int i = 0; i < MAXEXITS; i++)
		{m_Exits[i] = false;} m_Exits[Entrance] = 1; };

		inline Node(const Node& other) {
			if (&other != this)
			{
				for (int i = 0; i < MAXEXITS; i++)
				{
					m_Exits[i] = other.m_Exits[i];
				}
				m_GroupID = other.m_GroupID;
				m_ConnectAble = other.m_ConnectAble;
				m_CoordX = other.m_CoordX;
				m_CoordY = other.m_CoordY;
			}
		}
	//	inline const void AddExit(const Direction& exit) {  m_Exits[exit] = 1; }
		inline const void AddExit(const Direction& exit, const bool& reverted = 0) { 
			
			Direction tempDir = exit;
			if (reverted)
			{
				tempDir = (Direction)((~tempDir) & 0x3);
			}
			
			m_Exits[tempDir] = 1;
		
		
		}
		inline const uint16_t& GetX() const { return m_CoordX; }
		inline const uint16_t& GetY() const { return m_CoordY; }
		inline const bool* GetExits() { return m_Exits; }
		inline const bool& IsConnectAble() { return m_ConnectAble; }
		inline const uint16_t& GetGroupID() {return m_GroupID;}
		inline const uint8_t& MaxNrOfExits() { return MAXEXITS; }
		inline Node& operator= (const Node& other)
		{
			if (&other != this)
			{
				for (int i = 0; i < MAXEXITS; i++)
				{
					m_Exits[i] = other.m_Exits[i];
				}
				m_GroupID = other.m_GroupID;
				m_ConnectAble = other.m_ConnectAble;
			}


			return *this;
		}
		inline bool operator== (const Node& other) { return (other.m_GroupID == m_GroupID); }
		inline bool operator!= (const Node& other) { return (other.m_GroupID != m_GroupID); }
	};

	class BasePlacer
	{
	protected:
		static uint16_t s_GroupId;
		static float s_AvgDist;

		uint16_t m_Steps;
		uint16_t m_GroupID;

		uint16_t m_Xcoord;
		uint16_t m_Ycoord;

		bool m_ConnectAble;
		Direction m_CurrentDir;

		//temp
		bool stopWalk = false;

		bool m_SwappedDir[MAXEXITS-1];

		Node* m_CurrentNode;

		//Track list?


		//create pointer here?
		inline Node CreateNode() { return Node((Direction)((~m_CurrentDir) & 0x3), m_GroupID, m_Xcoord, m_Ycoord, m_ConnectAble); }
		inline void ChangeStep() { if (m_CurrentDir == North) { m_Ycoord++; } else if (m_CurrentDir == South) { m_Ycoord--; }	else if (m_CurrentDir == East) { m_Xcoord++; }	else { m_Xcoord--; }}
	public:
		inline BasePlacer(const Direction& CurrentDir, const uint16_t& Id,  Node* currentNode, const bool& ConnectAble) :
			m_CurrentDir(CurrentDir),
			m_GroupID(Id),
			m_Steps(0),
			m_ConnectAble(ConnectAble),
			m_CurrentNode(currentNode),
			m_Xcoord(currentNode->GetX()),
			m_Ycoord(currentNode->GetY())
			{}
		virtual bool Walk(Node*& in_ptr) = 0;

		inline const Direction& GetCurrentDir() { return m_CurrentDir; }

		inline const uint16_t& GetXcoord() {return m_Xcoord;}
		inline const uint16_t& GetYcoord() {return m_Ycoord;}

		inline void SetXcoord(const uint16_t& xCoord) {  m_Xcoord = xCoord; }
		inline void SetYcoord(const uint16_t& yCoord) {	m_Ycoord = yCoord;}
		inline void SetXYcoord(const uint16_t& xCoord, const uint16_t& yCoord) { SetXcoord(xCoord); SetYcoord(yCoord); }

		inline bool operator ==(const BasePlacer& other) { return (other.m_GroupID == m_GroupID); }
		inline bool ChangeDir(const uint8_t& modLeft, const uint8_t& modForward, const uint8_t& modRight)
		{
			LocalDirection locDir;
			bool returnValue = false;
			bool canChangeDir = true;
			//0 check
			if (modRight == modForward == modLeft)
			{
				locDir = (LocalDirection)(rand() % MAXEXITS - 1);
			}
			else
			{
				uint8_t Left = rand() % modLeft;
				uint8_t Right = rand() % modRight;
				uint8_t Forward = rand() % modForward;

				//Fix this later /optimize

				while (Left == Right == Forward)
				{
					Left = rand() % modLeft;
					Right = rand() % modLeft;
					Forward = rand() % modLeft;
				}

				if (Left < Right)
				{
					locDir = LocalDirection::Right;
				}
				else
				{
					locDir = LocalDirection::Left;
				}

				if (locDir == LocalDirection::Right)
				{
					if (Right < Forward)
					{
						locDir = LocalDirection::Forward;
					}
				}
				else
				{
					if (Left < Forward)
					{
						locDir = LocalDirection::Forward;
					}
				}

				if (locDir == LocalDirection::Right && m_SwappedDir[LocalDirection::Right])
				{
					if (m_SwappedDir[LocalDirection::Left] && m_SwappedDir[LocalDirection::Forward])
					{
						canChangeDir = false;
					}
					else if (m_SwappedDir[LocalDirection::Left])
					{
						locDir = LocalDirection::Forward;
					}
					else
					{
						locDir = LocalDirection::Left;
					}
				}
				else if (locDir == LocalDirection::Left && m_SwappedDir[LocalDirection::Left])
				{
					if (m_SwappedDir[LocalDirection::Right] && m_SwappedDir[LocalDirection::Forward])
					{
						canChangeDir = false;
					}
					else if (m_SwappedDir[LocalDirection::Right])
					{
						locDir = LocalDirection::Forward;
					}
					else
					{
						locDir = LocalDirection::Right;
					}
				}
				else if (locDir == LocalDirection::Forward && m_SwappedDir[LocalDirection::Forward])
				{
					if (m_SwappedDir[LocalDirection::Right] && m_SwappedDir[LocalDirection::Left])
					{
						canChangeDir = false;
					}
					else if (m_SwappedDir[LocalDirection::Right])
					{
						locDir = LocalDirection::Left;
					}
					else
					{
						locDir = LocalDirection::Right;
					}
				}



				if (locDir == LocalDirection::Right && !m_SwappedDir[LocalDirection::Right])
				{
					m_SwappedDir[LocalDirection::Right] = true;
				}

				else if (locDir == LocalDirection::Forward && !m_SwappedDir[LocalDirection::Forward])
				{
					m_SwappedDir[LocalDirection::Forward] = true;
				}

				else if (locDir == LocalDirection::Left && !m_SwappedDir[LocalDirection::Left])
				{
					m_SwappedDir[LocalDirection::Left] = true;
				}




			}
			if(canChangeDir)
			{
			//Fix this later /optimize
			if (m_CurrentDir == North)
			{
				if (locDir == LocalDirection::Right)
				{
					m_CurrentDir = East;
				}
				else if (locDir == LocalDirection::Left)
				{
					m_CurrentDir = West;
				}
			}

			else if (m_CurrentDir == South)
			{
				if (locDir == LocalDirection::Right)
				{
					m_CurrentDir = West;
				}
				else if (locDir == LocalDirection::Left)
				{
					m_CurrentDir = East;
				}
			}

			else if (m_CurrentDir == East)
			{
				if (locDir == LocalDirection::Right)
				{
					m_CurrentDir = South;
				}
				else if (locDir == LocalDirection::Left)
				{
					m_CurrentDir = North;
				}
			}

			else if (m_CurrentDir == West)
			{
				if (locDir == LocalDirection::Right)
				{
					m_CurrentDir = North;
				}
				else if (locDir == LocalDirection::Left)
				{
					m_CurrentDir = South;
				}
			}
			returnValue = true;
		}
			return returnValue;
		}
	};

	uint16_t BasePlacer::s_GroupId = 0;
	float BasePlacer::s_AvgDist = 0.0f;


	//class Seeker : public BasePlacer
	//{
	//private:

	//	

	//public:
	//	inline Seeker(const Direction& direction, const uint8_t& GroupID, const uint16_t& CoordX, const uint16_t& CoordY, const bool& connectAble = 1) : BasePlacer(direction,GroupID, CoordX,CoordY,connectAble) {}

	//	inline bool Walk(Node*& in_ptr)
	//	{
	//		bool returnValue = false;
	//		if (!stopWalk)
	//		{
	//			if (in_ptr == nullptr)
	//			{
	//				in_ptr = FY_NEW Node(CreateNode());
	//				m_CurrentNode = in_ptr;
	//				returnValue = true;
	//				for (uint8_t i = 0; i < (MAXEXITS - 1); i++)
	//				{
	//					m_SwappedDir[i] = 0;
	//				}
	//				m_Steps++;
	//			}
	//			else
	//			{
	//				//if (in_ptr->IsConnectAble() && in_ptr->GetGroupID() != m_GroupID) // TODO
	//				if (in_ptr->IsConnectAble())
	//				{
	//					in_ptr->AddExit(m_CurrentDir);
	//					m_CurrentNode = in_ptr;
	//					stopWalk = true;
	//				}
	//				else
	//				{
	//					returnValue = false;
	//				}
	//			}
	//		}
	//		return returnValue;
	//	}


	//};


	//class Loner : public BasePlacer
	//{
	//	inline Loner(const Direction& direction, const uint8_t& GroupID, const uint16_t& CoordX, const uint16_t& CoordY,const bool& connectAble = 0) : BasePlacer(direction, GroupID, CoordX,CoordY,connectAble) {}

	//	inline bool Walk(Node*& in_ptr)
	//	{
	//		bool returnValue = false;
	//		if (!stopWalk)
	//		{
	//			if (in_ptr == nullptr)
	//			{
	//				uint16_t k = DEADENDLENGTH; //??
	//				if (m_Steps < k)
	//				{
	//					in_ptr = FY_NEW Node(CreateNode());
	//					m_CurrentNode = in_ptr;
	//					returnValue = true;
	//					for (uint8_t i = 0; i < (MAXEXITS - 1); i++)
	//					{
	//						m_SwappedDir[i] = 0;
	//					}
	//					m_Steps++;
	//				}
	//				else
	//				{
	//					stopWalk = true;
	//				}
	//			}
	//			else
	//			{
	//				stopWalk = true;
	//			}
	//		}
	//		return returnValue;
	//	}


	//};
	/*
	//class HomeSeeker : public BasePlacer //TODO
	//{

	//};
	*/





	class Head : public BasePlacer //start here
	{

	private:
	 

	public:
		inline Head(const Direction& CurrentDir,  Node* StartNode) : BasePlacer(CurrentDir,  s_GroupId++, StartNode, 1) {}
		inline bool Walk(Node*& in_ptr)
		{
			bool returnValue = false;
			if (!stopWalk)
			{
				if (in_ptr == nullptr)
				{

					m_CurrentNode->AddExit(m_CurrentDir);
		
					ChangeStep();

					in_ptr = FY_NEW Node(CreateNode());
					m_CurrentNode = in_ptr;
					returnValue = true;
					for (uint8_t i = 0; i < (MAXEXITS - 1); i++)
					{
						m_SwappedDir[i] = 0;
					}
					m_Steps++;
				}
				else
				{
					//if (in_ptr->IsConnectAble() && in_ptr->GetGroupID() != m_GroupID) // TODO
					if (in_ptr->IsConnectAble())
					{
						m_CurrentNode->AddExit(m_CurrentDir);
						in_ptr->AddExit(m_CurrentDir,1);
						m_CurrentNode = in_ptr;
						stopWalk = true;
					}
					else
					{
						returnValue = false;
					}
				}
			}
			return returnValue;

		}



	};



	class NodeMap
	{
	private:
		inline static const std::string VERTEXSRC = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			
			layout(location = 44) uniform mat4 u_ViewOrtho;
			layout(location = 33) uniform mat4 u_ModelMat;
			
			void main()
			{				
				gl_Position = u_ViewOrtho *u_ModelMat * vec4(vsInPos, 1.0f);
			}
		)";
		inline static const std::string FRAGMENTSRC = R"(
			#version 440 core

			layout(location = 0) out vec4 fsOutCol;
			layout(location = 55) in vec3 renderCol;
			
			void main()
			{
				fsOutCol = vec4( renderCol, 1.0f ); 
			}
		)";



	private:


		std::list<BasePlacer*> m_WalkerList;
		Node*** m_Grid;
		uint16_t m_Width;
		uint16_t m_Height;
		std::string m_Seed;

		std::vector<Node> m_Nodes;

		//temp
		inline void TranslateMap()
		{
			m_Nodes.erase(m_Nodes.begin(), m_Nodes.end());
			for (int i = 0; i < m_Width; i++)
			{
				for (int j = 0; j < m_Height; j++)
				{
					if (m_Grid[i][j] != nullptr)
					{
						//Todo move this inside generator
						m_Nodes.emplace_back(*m_Grid[i][j]);
					}
				}
			}
		}
		inline void DeleteWalkerList()
		{
			std::list<BasePlacer*>::iterator it = m_WalkerList.begin();
			while (it != m_WalkerList.end())
			{
				if ((*it) != nullptr)
				{
					delete (*it);
				}
				it++;
			}
			m_WalkerList.erase(m_WalkerList.begin(), m_WalkerList.end());
		}

	public:
		inline NodeMap() {};
		inline NodeMap(const uint16_t& width, const uint16_t& heigth, std::string Seed = "baseSeed") : m_Seed(Seed), m_Width(width), m_Height(heigth), m_Grid(nullptr)
		{
			std::string::size_type ty;
			std::stringstream sStrm(Seed);

			int x;
			sscanf(Seed.c_str(), "%x", &x);
			srand(x);
			m_Grid = FY_NEW Node**[width];

			for (int i = 0; i < width; i++)
			{
				m_Grid[i] = FY_NEW Node* [heigth];
				for (int j = 0; j < heigth; j++)
				{
					m_Grid[i][j] = nullptr;
				}
			}

		}
		inline ~NodeMap() 
		{
		
			for (int i = 0; i < m_Width; i++)
			{
				
				for (int j = 0; j < m_Height; j++)
				{
					if (m_Grid[i][j] != nullptr)
					{
						delete m_Grid[i][j];
						m_Grid[i][j] = nullptr;
					}
				}
				delete[] m_Grid[i];
			}
			delete[] m_Grid;
		}

		inline Node*** GetMap() {return m_Grid;}
		inline const std::vector<Node>& GetNodes() { return m_Nodes; }
		inline void WriteMapToFile()
		{
			char** grid = FY_NEW char* [(int)m_Width *3];
			for (int i = 0; i < m_Width *3; i++)
			{
				grid[i] = FY_NEW char[(int)m_Height* 3];

			}

			std::ofstream file;
			file.open("map.txt");


			for (int i = 0; i < m_Nodes.size(); i++)
			{
				int tempX = m_Nodes.at(i).GetX() * 3;
				int tempY = m_Nodes.at(i).GetY() * 3;

				grid[tempX][tempY] = 'x';


				const bool* tmpptr = m_Nodes.at(i).GetExits();

				if (tmpptr[0])
				{
					grid[tempX][tempY-1] = '1';
				}
				if (tmpptr[1])
				{
					grid[tempX+1][tempY] = '1';
				}
				if (tmpptr[2])
				{
					grid[tempX-1][tempY] = '1';
				}
				if (tmpptr[3])
				{
					grid[tempX][tempY + 1] = '1';
				}


			}


			for (int i = 0; i < m_Height * 3; i++)
			{
				for (int j = 0; j < m_Width * 3; j++)
				{
					if (grid[i][j] != '1' && grid[i][j] != 'x')
					{
						grid[i][j] = '0';
					}


					file << grid[i][j];

				}
				file << "\n";
			}



			for (int i = 0; i < m_Width * 3; i++)
			{
				delete[] grid[i];
			}
			delete[] grid;

		}
		inline void GenereateMap()
		{
			int cenX = m_Width / 2;
			int cenY = m_Height / 2;
			

			m_Grid[cenX][cenY] = FY_NEW Node(North,0,cenX,cenY,0);
			m_WalkerList.push_back(FY_NEW Head(STARTDIR, m_Grid[cenX][cenY]));



			bool endWalk = false;
			while (!endWalk)
			{

				std::list<BasePlacer*>::iterator it = m_WalkerList.begin();

				while (it != m_WalkerList.end())
				{
					


						uint16_t xPos = (*it)->GetXcoord();
						uint16_t yPos = (*it)->GetYcoord();

						if ((*it)->GetCurrentDir() == North || (*it)->GetCurrentDir() == South)
						{
							if ((*it)->GetCurrentDir() == North)
							{
								yPos++;
							}
							else
							{
								yPos--;
							}
						}
						else
						{
							if ((*it)->GetCurrentDir() == East)
							{
								xPos++;
							}
							else
							{
								xPos--;
							}
						}


						bool CangedDir = false;
						if (xPos < m_Width && yPos < m_Height)
						{
							if (!(*it)->Walk(m_Grid[xPos][yPos]))
							{
								endWalk = true;
								//CangedDir = (*it)->ChangeDir(LocRw, LocLw, LocFw);
							}
						}
						else
						{
							//change dir
							endWalk = true;
						}

						if (!endWalk && !CangedDir)
						{
							if ((rand() % CngDir) == 0)
							{
								CangedDir = (*it)->ChangeDir(LocR, LocL, LocF);
							}
						}
					
					it++;
				}
			}
			

			TranslateMap();
		//	WriteMapToFile();
			DeleteWalkerList();
		
		
		}
		void RenderMap();

		


	};

}
#endif // ! TESTMAP_HPP


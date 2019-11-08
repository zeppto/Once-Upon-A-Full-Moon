#ifndef  NODEMAP_HPP
#define NODEMAP_HPP



namespace Frosty
{

#define MAXEXITS (uint8_t)4

	enum Direction{
		North = (int8_t)3,
		West = (int8_t)2,
		East = (int8_t)1,
		South = (int8_t)0
	};



	class Node
	{
	private:
		
		bool m_ConnectAble;
		uint16_t m_GroupID;
		bool m_Exits [MAXEXITS];

	public:
		inline Node(const Direction& Entrance,const uint16_t& GrpID,const bool& ConnectAble) : m_GroupID(GrpID), m_ConnectAble(ConnectAble){ m_Exits[Entrance] = 1; };
		inline const bool& AddExit(const Direction& exit) { return m_Exits[exit] = !m_Exits[exit] ? 1 : 0; }
		inline const bool* GetExits() { return m_Exits; }
		inline const bool& IsConnectAble() { return m_ConnectAble; }
		inline const uint16_t& GetGroupID() {return m_GroupID;}
		inline const uint8_t& MaxNrOfExits() { return MAXEXITS; }
	};

	class BasePlacer
	{
	protected:
		static uint16_t s_GroupId;
		static float s_AvgDist;

		uint16_t m_Steps;
		uint16_t m_GroupID;

		bool m_ConnectAble;
		Direction m_CurrentDir;

		bool m_SwappedDir[MAXEXITS-1];

		std::shared_ptr<Node> m_CurrentNode;

		//Track list?


	public:
		inline BasePlacer(const Direction& CurrentDir,const uint16_t& Id, const bool& ConnectAble) : m_CurrentDir(CurrentDir),m_GroupID(Id), m_Steps(0), m_ConnectAble(ConnectAble), m_CurrentNode(nullptr), m_NrSwappedDir(0){}
		virtual bool Walk(std::shared_ptr<Node>& in_ptr) = 0;

		inline bool operator ==(const BasePlacer& other) { return (other.m_GroupID == m_GroupID); }
		inline Node& CreateNode() { return Node((Direction)~m_CurrentDir, m_GroupID, m_ConnectAble); }
		inline void ChangeDir(const uint8_t& modRight, const uint8_t& modForward, const uint8_t& modLeft)
		{

			//0 check

			uint8_t Left = rand() % modLeft;
			uint8_t Right = rand() % modRight;
			uint8_t Forward = rand() % modForward;

			while (Left == Right == Forward)
			{
				Left = rand() % modLeft;
				Right = rand() % modLeft;
				Forward = rand() % modLeft;
			}



			m_CurrentDir
			
		
		
		}

	};

	uint16_t BasePlacer::s_GroupId = 0;
	float BasePlacer::s_AvgDist = 0.0f;


	class Seeker : public BasePlacer
	{
	private:

		

	public:
		inline Seeker(const Direction& direction, const uint8_t& GroupID,const bool& connectAble = 1) : BasePlacer(direction,GroupID, connectAble) {}

		inline bool Walk(std::shared_ptr<Node>& in_ptr)
		{
			if (in_ptr == nullptr)
			{
				in_ptr = std::make_shared<Node>(FY_NEW Node(CreateNode()));
			}
			else
			{

			}
		}


	};


	class Loner : public BasePlacer
	{



	};

	class HomeSeeker : public BasePlacer
	{



	};



	class Head : public BasePlacer
	{

	private:


	public:
		inline Head() : BasePlacer(s_GroupId++) {}




	};




	//Nodemap
	class NodeMap
	{


	private:


		std::vector<std::vector<int>> m_Grid;



	public:
		inline NodeMap(const uint16_t& width, const uint16_t& heigth, std::string Seed = "BaseSeed")
		{
			std::string::size_type ty;
			srand(std::stoi(Seed, &ty));
			m_Grid.resize(width);
			for (int i = 0; i < width; i++)
			{
				m_Grid[i].resize(heigth);
			}

		}
		~NodeMap();







	};

}
#endif // ! TESTMAP_HPP


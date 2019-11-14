#include <mcspch.hpp>
#include "Game.hpp"

#include "StateMachine/StateMachine.hpp"


//#include "LevelHandeler/LevelFileFormat.hpp"



namespace MCS
{
	Game::Game()
	{



	}

	void Game::RunGame()
	{
		//Menu state




		//Loadstate




		//GameState




		Run();
	}

	Game::~Game()
	{

	}
}


/*
[22:33:06] FROSTY:
		-----------Entity Info-----------
		Index   Id      Address                 Refs
		0       1       000002900F698CB0        7
		1       3       000002900F6976F0        6
		2       4       000002900F697750        6
		3       5       000002900F698D70        20
		4       6       000002900F698A70        10
		5       7       000002900F697990        12
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Transform Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F97A330        1               000002900F698CB0                7
		2       000002900F97A3B0        7               000002900F697990                12
		3       000002900F97A430        3               000002900F6976F0                6
		4       000002900F97A4B0        4               000002900F697750                6
		5       000002900F97A530        5               000002900F698D70                20
		6       000002900F97A5B0        6               000002900F698A70                10
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Camera Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F99A428        1               000002900F698CB0                7
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Light Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900FA104A0        3               000002900F6976F0                6
		2       000002900FA104D0        4               000002900F697750                6
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Mesh Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F9DC3B8        7               000002900F697990                12
		2       000002900F9DC3E0        5               000002900F698D70                20
		3       000002900F9DC408        6               000002900F698A70                10
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Material Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F9E64A8        7               000002900F697990                12
		2       000002900F9E6550        5               000002900F698D70                20
		3       000002900F9E65F8        6               000002900F698A70                10
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Player Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F69DFC0        5               000002900F698D70                20
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Physics Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6AE040        5               000002900F698D70                20
		2       000002900F6AE090        6               000002900F698A70                10
		3       000002900F6AE0E0        7               000002900F697990                12
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Dash Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6C2088        5               000002900F698D70                20
		----------------Done----------------


[22:33:06] FROSTY:
		-----------BasicAttack Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6CC0F0        5               000002900F698D70                20
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Health Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6D4168        5               000002900F698D70                20
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Inventory Component Manager-----------
		Index   Component Address       Entity Id       Entity Address          Entity Refs
		1       000002900F6DE210        5               000002900F698D70                20
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Camera System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				2               000002900F697690                2       0
				7               000002900F697990                12      0
				1               000002900F698CB0                7       1
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A330        1               000002900F698CB0                7
				1       000002900F99A428        1               000002900F698CB0                7

				-----------Done-----------
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Light System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				2               000002900F697690                2       0
				3               000002900F6976F0                6       1
				4               000002900F697750                6       2
				7               000002900F697990                12      0
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A430        3               000002900F6976F0                6
				1       000002900FA104A0        3               000002900F6976F0                6

				2       000002900F97A4B0        4               000002900F697750                6
				2       000002900FA104D0        4               000002900F697750                6

				-----------Done-----------
		----------------Done----------------


[22:33:06] FROSTY:
		-----------Render System Info-----------
				---------Entity Map---------
				Entity Id       Entity Address          Entity Refs     Array Index
				7               000002900F697990                12      4
				6               000002900F698A70                10      3
				5               000002900F698D70                20      2
				-----------Done-----------
				------Component Array(s)------

				Index   Component Address       Entity Id       Entity Address          Entity Refs
				1       000002900F97A3B0        7               000002900F697990                12
				1       000002900F9DC3B8        7               000002900F697990                12
				1       000002900F9E64A8        7               000002900F697990                12

				2       000002900F97A530        5               000002900F698D70                20
				2       000002900F9DC3E0        5               000002900F698D70                20
				2       000002900F9E6550        5               000002900F698D70                20

				3       000002900F97A5B0        6               000002900F698A70                10
				3       000002900F9DC408        6               000002900F698A70                10
				3       000002900F9E65F8        6               000002900F698A70                10

				-----------Done-----------
		----------------Done----------------
*/
#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"
//#include "LevelHandeler/MapGenerator.hpp"

//temp
//#include "LevelHandeler/LevelsHardCoded.hpp"
#include "Systems/FollowSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/AttackSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/LevelSystem.hpp"
//#include "Systems/HealthBarSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"




namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();
		//std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<AttackSystem>();
		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		//world->AddSystem<HealthBarSystem>();
		world->AddSystem<ParticleSystem>();
		world->AddSystem<BossBehaviorSystem>();

		//MapGenerator map;
		//map.generateMap();
		//Room myRoome = map.getRoom(glm::ivec2(11, 15));

		//for (int i = 0; i < 30; i++)
		//{
		//	for (int j = 0; j < 30; j++)
		//	{
		//		auto& cube = world->CreateEntity();
		//		auto& cubeTranform = world->GetComponent<Frosty::ECS::CTransform>(cube);
		//		cubeTranform.Position = glm::vec3(i,2,j);
		//		auto& cubeMaterial = world->AddComponent<Frosty::ECS::CMaterial>(cube, Frosty::AssetManager::GetShader("Texture2D"));
		//		Room myRoome = map.getRoom(glm::ivec2(i, j));
		//		int rotation = 0;
		//		//cubeMaterial.Albedo = glm::vec4( 0.1f, 0.8f, 0.1f, 1.0f);
		//		cubeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(map.getRoomTextur(glm::ivec2(i,j), &rotation));
		//		cubeTranform.Rotation = glm::vec3(0, rotation, 0);
		//		if (i == 10 && j == 15)
		//			cubeTranform.Scale = glm::vec3(0.5f, 0.5f, 0.5f);
		//		//std::string cubeName = "cube" + std::to_string(((rand() % NrOfcubeTypes) + 1));
		//		//cubeMaterial.DiffuseTexture = Frosty::AssetManager::GetTexture2D(cubeName);

		//		//cubeName = "cube" + std::to_string(((rand() % NrOfcubeTypes) + 1));
		//		world->AddComponent<Frosty::ECS::CMesh>(cube, Frosty::AssetManager::GetMesh("Plane"));
		//	}
		//}

		world->Awake();
		
		//auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 100.0f, 1.0f, 100.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("BlendShader"));
		//planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Grass");
		//planeMat.BlendMapTexture = Frosty::AssetManager::GetTexture2D("blendMap_Test");	// why is this texture a problem
		//planeMat.BlendTexture1 = Frosty::AssetManager::GetTexture2D("StoneGround");
		//planeMat.BlendTexture2 = Frosty::AssetManager::GetTexture2D("Dirt");
		
		//auto& testEntity = world->CreateEntity({ -47.5f, 0.01f, 47.5f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 1.0f, 5.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(testEntity, Frosty::AssetManager::GetMesh("pPlane1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(testEntity, Frosty::AssetManager::GetShader("FlatColor"));
		
		// LIGHT 1
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		
		// LIGHT 2
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto& lightTransform2 = world->GetComponent<Frosty::ECS::CTransform>(light2);
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

		// WEAPON 1 (testing)
		auto& weapon = world->CreateEntity({ 0.f, 0.f, 0.f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CWeapon>(weapon, Frosty::ECS::CWeapon::WeaponType::Arrow, "2Default", 10.f, 0.1f);
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);

		// WEAPON 2 (testing)
		auto& weapon2 = world->CreateEntity({ 4.f, 0.f, 0.f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		world->AddComponent<Frosty::ECS::CMesh>(weapon2, Frosty::AssetManager::GetMesh("pCylinder1"));
		world->AddComponent<Frosty::ECS::CMaterial>(weapon2, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CWeapon>(weapon2, Frosty::ECS::CWeapon::WeaponType::Sword, "1Default", 10.f, 0.2f);
		auto& weaponMaterialComp = world->GetComponent<Frosty::ECS::CMaterial>(weapon2);
		weaponMaterialComp.Albedo = glm::vec4(0.f, 0.f, 1.f, 1.f);
		auto& weaponComp2 = world->GetComponent<Frosty::ECS::CWeapon>(weapon2);

		// PLAYER
		auto& player = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		// PLAYER 2 (testing)
		auto& player2 = world->CreateEntity({ 25.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		auto& playerTransform2 = world->GetComponent<Frosty::ECS::CTransform>(player2);
		world->AddComponent<Frosty::ECS::CMesh>(player2, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat2 = world->AddComponent<Frosty::ECS::CMaterial>(player2, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat2.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat2.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat2.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player2, &weaponComp2);
		world->AddComponent<Frosty::ECS::CPhysics>(player2, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player2);
		world->AddComponent<Frosty::ECS::CHealth>(player2);
		world->AddComponent<Frosty::ECS::CInventory>(player2);

		// WALL
		auto& wall = world->CreateEntity({ 0.0f, 5.0f, -3.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 1.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));

		// ENEMY 1
		auto& enemy = world->CreateEntity({ 27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CEnemy>(enemy);
		world->AddComponent<Frosty::ECS::CFollow>(enemy, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemy);
		auto& bossComponent = world->AddComponent<Frosty::ECS::CBoss>(enemy);
		bossComponent.TargetList.emplace_back(player);
		
		// ENEMY 2
		auto& enemy2 = world->CreateEntity({ -27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemy2, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy2, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemy2, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CEnemy>(enemy2);
		world->AddComponent<Frosty::ECS::CFollow>(enemy2, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemy2);

		// TREE
		auto& tree = world->CreateEntity({ 17.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
		auto& treeMat = world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
		treeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");
		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"));
		
		//navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));
		
		PushLayer(FY_NEW InspectorLayer());
		world->PrintWorld();

		//world->AddToDestroyList(plane);
		


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
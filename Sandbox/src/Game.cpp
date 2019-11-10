#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"
#include "Systems/FollowSystem.hpp"
#include "Systems/BasicAttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/ArrowSystem.hpp"
#include "Systems/SwordSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
//#include "Systems/PlayerAttackSystem.hpp"
//#include "Systems/EnemyAttackSystem.hpp"
//#include "Systems/HealthBarSystem.hpp"



namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();
		//std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		world->AddSystem<MCS::CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<BasicAttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<ArrowSystem>();
		world->AddSystem<SwordSystem>();
		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		//world->AddSystem<CollisionSystem>();		// Have this after MovementSystem
		//world->AddSystem<DashSystem>();
		//world->AddSystem<PlayerAttackSystem>();
		//world->AddSystem<EnemyAttackSystem>();
		//world->AddSystem<SpawnSystem>();
		//world->AddSystem<HealthBarSystem>();
		//world->AddSystem<ParticleSystem>();

		world->Awake();



		
		auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 100.0f, 1.0f, 100.0f }, false);
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("BlendShader"));
		planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Grass");
		planeMat.BlendMapTexture = Frosty::AssetManager::GetTexture2D("blendMap_Test");	// why is this texture a problem
		planeMat.BlendTexture1 = Frosty::AssetManager::GetTexture2D("StoneGround");
		planeMat.BlendTexture2 = Frosty::AssetManager::GetTexture2D("Dirt");
		
		//auto& testEntity = world->CreateEntity({ -47.5f, 0.01f, 47.5f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 1.0f, 5.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(testEntity, Frosty::AssetManager::GetMesh("pPlane1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(testEntity, Frosty::AssetManager::GetShader("FlatColor"));
		
		// Night Light
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		
		// Night Light (makeshift reflection)
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto& lightTransform2 = world->GetComponent<Frosty::ECS::CTransform>(light2);
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

		auto& player = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CBasicAttack>(player, Frosty::ECS::CBasicAttack::AttackType::Range);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		auto& wall = world->CreateEntity({ 0.0f, 5.0f, -3.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 1.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));
		
		auto& tree = world->CreateEntity({ 17.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
		auto& treeMat = world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
		treeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");
		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"));
		
		auto& enemyA = world->CreateEntity({ 27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CFollow>(enemyA, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemyA);

		auto& enemyB = world->CreateEntity({ -27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemyB, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemyB, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemyB, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CFollow>(enemyB, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemyB);

		navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));
		
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
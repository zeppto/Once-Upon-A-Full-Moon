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
#include "Systems/BasicAttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/ArrowSystem.hpp"
#include "Systems/SwordSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/LevelSystem.hpp"
//#include "Systems/PlayerAttackSystem.hpp"
//#include "Systems/EnemyAttackSystem.hpp"
//#include "Systems/HealthBarSystem.hpp"

#include "LevelHandeler/LevelFileFormat.hpp"



namespace MCS
{
	Game::Game()
	{

		auto& world = Application::Get().GetWorld();
		//std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		world->AddSystem<LevelSystem>();
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
		world->AddSystem<ParticleSystem>();

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
		//navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));
		
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

		//generateTrees();
		//generateBorders();
		//generatePlanes();

		//int rotation = 0;
		//std::string texture = map.getRoomTextur(glm::ivec2(11, 15), &rotation);
		//Level::Room(myRoome.sideExits[0], myRoome.sideExits[1], myRoome.sideExits[2], myRoome.sideExits[3], texture, rotation);

		LevelFileFormat myLevelFileFormat;
		
		auto& player = world->CreateEntity();
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		playerTransform.Scale = glm::vec3(2.0f, 2.0f, 2.0f);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 20.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CBasicAttack>(player, Frosty::ECS::CBasicAttack::AttackType::Range);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		auto& wall = world->CreateEntity({ 0.0f, 5.0f, -3.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 1.0f });
		world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));

		myLevelFileFormat.AddEntity(player);
		myLevelFileFormat.AddEntity(wall);

		

		//auto& enemy = world->CreateEntity({ 27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		//world->AddComponent<Frosty::ECS::CFollow>(enemy, &playerTransform);
		//world->AddComponent<Frosty::ECS::CHealth>(enemy);

		//auto& enemy2 = world->CreateEntity({ -27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(enemy2, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(enemy2, Frosty::AssetManager::GetShader("FlatColor"));
		//world->AddComponent<Frosty::ECS::CPhysics>(enemy2, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		//world->AddComponent<Frosty::ECS::CFollow>(enemy2, &playerTransform);
		//world->AddComponent<Frosty::ECS::CHealth>(enemy2);


		//auto& tree = world->CreateEntity({ 17.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
		//auto& treeMat = world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
		//treeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");
		//world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"));
		//

		PushLayer(FY_NEW InspectorLayer());
	}

	Game::~Game()
	{

	}
}
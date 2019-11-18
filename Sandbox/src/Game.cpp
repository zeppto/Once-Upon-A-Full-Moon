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
#include "Systems/AttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/ChestSystem.hpp"
#include "Systems/LootingSystem.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/HealthBarSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"
#include "Systems/GUISystem.hpp"

//#include "LevelHandeler/LevelFileFormat.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();
		// Add systems
		world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<AttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();

		world->AddSystem<HealthBarSystem>();

		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		retSystem = world->AddSystem<ParticleSystem>();
		ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);
		world->AddSystem<BossBehaviorSystem>();
		world->AddSystem<GUISystem>();
		world->AddSystem<ChestSystem>();
		world->AddSystem<LootingSystem>();

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
		particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));
		
		auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 60.0f, 1.0f, 60.0f });
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("BlendShader"));
		planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Grass");
		planeMat.BlendMapTexture = Frosty::AssetManager::GetTexture2D("blendMap_Test");	// why is this texture a problem
		planeMat.BlendTexture1 = Frosty::AssetManager::GetTexture2D("StoneGround");
		planeMat.BlendTexture2 = Frosty::AssetManager::GetTexture2D("Dirt");
		
		// LIGHT 1
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		
		//LIGHT 2
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto& lightTransform2 = world->GetComponent<Frosty::ECS::CTransform>(light2);
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

		// WEAPON 1
		auto& weapon = world->CreateEntity({ 0.f, 0.f, 0.f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CWeapon>(weapon, Frosty::ECS::CWeapon::WeaponType::Arrow, 1, 10.f);
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);

		// PLAYER
		auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);	// <-- Give player a weapon
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 40.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		// WALL
		auto& wall = world->CreateEntity({ -16.0f, 5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 20.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));

		// ENEMY 1
		auto& enemy = world->CreateEntity({ 27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemy, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemy, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CEnemy>(enemy, &playerTransform);
		//world->AddComponent<Frosty::ECS::CFollow>(enemy, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemy);

		auto& bossComponent = world->AddComponent<Frosty::ECS::CBoss>(enemy);

		bossComponent.TargetList.emplace_back(player);

		// ENEMY 2
		auto& enemy2 = world->CreateEntity({ -27.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemy2, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(enemy2, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(enemy2, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CEnemy>(enemy2, &playerTransform);
		world->AddComponent<Frosty::ECS::CFollow>(enemy2, &playerTransform);
		world->AddComponent<Frosty::ECS::CHealth>(enemy2);

		// TREE
		auto& tree = world->CreateEntity({ 17.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("tree1"));
		auto& treeMat = world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("Texture2D"));
		treeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Tree1");
		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("tree1"));

		// CHEST 1
		auto& chest = world->CreateEntity({ 0.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(chest, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(chest, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(chest, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CHealth>(chest, 2.0f);
		world->AddComponent<Frosty::ECS::CChest>(chest);
		
		// CHEST 2
		auto& chest2 = world->CreateEntity({ 5.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(chest2, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(chest2, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(chest2, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CHealth>(chest2, 2.0f);
		world->AddComponent<Frosty::ECS::CChest>(chest2);

		// CHEST 3
		auto& chest3 = world->CreateEntity({ 10.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(chest3, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(chest3, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(chest3, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CHealth>(chest3, 2.0f);
		world->AddComponent<Frosty::ECS::CChest>(chest3);

		// CHEST 4
		auto& chest4 = world->CreateEntity({ 15.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(chest4, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(chest4, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(chest4, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CHealth>(chest4, 2.0f);
		world->AddComponent<Frosty::ECS::CChest>(chest4);

		// CHEST 5
		auto& chest5 = world->CreateEntity({ 20.0f, 1.0f, 25.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		world->AddComponent<Frosty::ECS::CMesh>(chest5, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(chest5, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(chest5, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);
		world->AddComponent<Frosty::ECS::CHealth>(chest5, 2.0f);
		world->AddComponent<Frosty::ECS::CChest>(chest5);

		// TEXT
		auto& GUI = world->CreateEntity();
		Frosty::UILayout uiLayout(3, 1);
		uiLayout.AddText(glm::vec2(25.0f, 20.0f), "Hello team");
		uiLayout.AddText(glm::vec2(20.0f, 700.0f), "uwu", glm::vec3(1.0f, 0.0f, 1.0f), 0.25f);
		uiLayout.AddText(glm::vec2(25.0f, 220.0f), "1234!", glm::vec3(0.5f, 0.1f, 0.9f), 1.5f);
		world->AddComponent<Frosty::ECS::CGUI>(GUI, uiLayout);
		
		navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));
		
		world->PrintWorld();


#ifdef FY_DEBUG
		PushLayer(FY_NEW InspectorLayer());
#else
		Application::Get().StartGame(true);
#endif // FY_DEBUG
	}


	Game::~Game()
	{
	}
}

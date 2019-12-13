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
#include "Systems/LootingSystem.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/HealthBarSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/AISystem.hpp"

//#include "LevelHandeler/LevelFileFormat.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();
		// Add systems
		//world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<AttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<AnimationSystem>();
		world->AddSystem<HealthBarSystem>();
		world->AddSystem<AISystem>();
		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		retSystem = world->AddSystem<ParticleSystem>();
		ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);


		world->AddSystem<BossBehaviorSystem>();
		world->AddSystem<GUISystem>();
		world->AddSystem<LootingSystem>();



		world->Awake();


		// SCENE 1
		// PLANE
		auto& plane = world->CreateEntity({ 0.0f, 0.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 200.0f, 1.0f, 125.0f }, true);
		auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("Texture2D"));
		planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("brown_mud_diffuse");
		//planeMat.Albedo = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
		world->AddToGroup(plane);
		
		// LIGHT 1
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -1.0, 1.0);
		//LIGHT 2
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto& DLight2 = world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));
		DLight2.Direction = glm::vec3(-1.0f, -1.0, 1.0);

		// PLAYER WEAPON 
		auto& playerWeapon = world->CreateEntity({ 0.f, 0.f, 0.f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		auto& playerWeaponComp = world->AddComponent<Frosty::ECS::CWeapon>(playerWeapon, Frosty::ECS::CWeapon::WeaponType::Arrow, 1, 9);
		//auto& playerWeaponComp = world->GetComponent<Frosty::ECS::CWeapon>(playerWeapon);

		// PLAYER
		//auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		//auto& player = world->CreateEntity({ -90.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		//auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		//world->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("NewRun");
		//world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("ScarRun:model:scarlet"));
		//auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
		//playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		//playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		//playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		//world->AddComponent<Frosty::ECS::CPlayer>(player, &playerWeaponComp);	// <-- Give player a weapon
		//world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		//world->AddComponent<Frosty::ECS::CDash>(player);
		//world->AddComponent<Frosty::ECS::CHealth>(player, 20);
		//world->AddComponent<Frosty::ECS::CInventory>(player);
		//world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));

		auto& player = world->CreateEntity({ -90.0f, 1.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 3.0f, 2.0f });
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("pCylinder1"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		playerMat.Albedo = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("pCylinder1"), 6.0f);

		// Cam target
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &planeTransform;

		// ENEMY WEAPON 
		auto& enemyWeaponA = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		auto& enemyWeaponCompA = world->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Arrow , 1, 1);
		enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
		//enemyWeaponCompA.MaxAttackRange = 5.0f;
		//enemyWeaponCompA.MinAttackRange = 0.0f;
		enemyWeaponCompA.MaxAttackRange = 0.0f;
		enemyWeaponCompA.MinAttackRange = 0.0f;
		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
		enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow

		// ENEMY A
		auto& enemyA = world->CreateEntity({ 91.0f, 1.5f, -54.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 3.0f, 2.0f });
		//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
		world->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("pCube1"));
		auto& enemyMatA = world->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("FlatColor"));
		enemyMatA.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("pCube1"), 10.0f);
		auto& enemyComp = world->AddComponent<Frosty::ECS::CEnemy>(enemyA, &playerTransform, &enemyWeaponCompA, 0.1f);
		//world->AddComponent<Frosty::ECS::CHealth>(enemyA, 10);


		// TREE
		auto& tree1 = world->CreateEntity({ 66.0f, 3.0f, -16.0f }, { 0.0f, 0.0f, 0.0f }, { 17.0f, 6.0f, 41.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree1, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree1Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree1, Frosty::AssetManager::GetShader("FlatColor"));
		tree1Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree1, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree2 = world->CreateEntity({ 54.0f, 3.0f, -51.0f }, { 0.0f, 0.0f, 0.0f }, { 33.0f, 6.0f, 10.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree2, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree2Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree2, Frosty::AssetManager::GetShader("FlatColor"));
		tree2Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree2, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree3 = world->CreateEntity({ 30.0f, 3.0f, 6.5f }, { 0.0f, 0.0f, 0.0f }, { 35.0f, 6.0f, 45.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree3, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree3Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree3, Frosty::AssetManager::GetShader("FlatColor"));
		tree3Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree3, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree4 = world->CreateEntity({ 18.0f, 3.0f, 47.0f }, { 0.0f, 0.0f, 0.0f }, { 65.0f, 6.0f, 14.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree4, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree4Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree4, Frosty::AssetManager::GetShader("FlatColor"));
		tree4Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree4, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree5 = world->CreateEntity({ 72.0f, 3.0f, 31.0f }, { 0.0f, 0.0f, 0.0f }, { 20.0f, 6.0f, 22.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree5, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree5Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree5, Frosty::AssetManager::GetShader("FlatColor"));
		tree5Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree5, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree6 = world->CreateEntity({ 22.0f, 3.0f, -41.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 6.0f, 30.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree6, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree6Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree6, Frosty::AssetManager::GetShader("FlatColor"));
		tree6Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree6, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree7 = world->CreateEntity({ -34.0f, 3.0f, 22.0f }, { 0.0f, 0.0f, 0.0f }, { 35.0f, 6.0f, 45.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree7, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree7Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree7, Frosty::AssetManager::GetShader("FlatColor"));
		tree7Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree7, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree8 = world->CreateEntity({ -2.0f, 3.0f, 20.5f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 6.0f, 10.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree8, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree8Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree8, Frosty::AssetManager::GetShader("FlatColor"));
		tree8Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree8, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree9 = world->CreateEntity({ -74.0f, 3.0f, 26.5f }, { 0.0f, 0.0f, 0.0f }, { 24.0f, 6.0f, 13.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree9, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree9Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree9, Frosty::AssetManager::GetShader("FlatColor"));
		tree9Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree9, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree10 = world->CreateEntity({ -16.0f, 3.0f, -25.0f }, { 0.0f, 0.0f, 0.0f }, { 37.0f, 6.0f, 29.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree10, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree10Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree10, Frosty::AssetManager::GetShader("FlatColor"));
		tree10Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree10, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree11 = world->CreateEntity({ -70.0f, 3.0f, -25.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 6.0f, 60.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree11, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree11Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree11, Frosty::AssetManager::GetShader("FlatColor"));
		tree11Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree11, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& tree12 = world->CreateEntity({ -50.0f, 3.0f, -23.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 6.0f, 10.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree12, Frosty::AssetManager::GetMesh("pCube1"));
		auto& tree12Mat = world->AddComponent<Frosty::ECS::CMaterial>(tree12, Frosty::AssetManager::GetShader("FlatColor"));
		tree12Mat.Albedo = glm::vec4(0.436f, 0.295f, 0.0f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(tree12, Frosty::AssetManager::GetBoundingBox("pCube1"));

		navSystem->InitiateGridMap(planeTransform);
		planeTransform.Position.y = -0.2f;


//ifdef FY_DEBUG
	PushLayer(FY_NEW InspectorLayer());
//else
//		Application::Get().StartGame(true);
//endif // FY_DEBUG
	}

	Game::~Game()
	{
	}
}

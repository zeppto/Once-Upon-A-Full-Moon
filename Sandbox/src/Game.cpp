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
#include "Systems/WeaponSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/ChestSystem.hpp"
#include "Systems/LootingSystem.hpp"
#include "Systems/LevelSystem.hpp"
//#include "Systems/HealthBarSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"
#include "Systems/GUISystem.hpp"

//#include "LevelHandeler/LevelFileFormat.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& world = Application::Get().GetWorld();
		//std::srand((unsigned)std::time(0)); //We already seed in main. We shouldn't seed twice
		// Add systems
		//world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<BasicAttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<WeaponSystem>();
		//world->AddSystem<HealthBarSystem>();
		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
		retSystem = world->AddSystem<ParticleSystem>();
		ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);
		world->AddSystem<BossBehaviorSystem>();
		world->AddSystem<GUISystem>();
		world->AddSystem<ChestSystem>();
		world->AddSystem<LootingSystem>();


		world->Awake();
		
		auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 60.0f, 1.0f, 60.0f });
		auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("FlatColor"));
		planeMat.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &planeTransform;
		
		//auto& testEntity = world->CreateEntity({ -47.5f, 0.01f, 47.5f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 1.0f, 5.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(testEntity, Frosty::AssetManager::GetMesh("pPlane1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(testEntity, Frosty::AssetManager::GetShader("FlatColor"));
		
		// Night Light
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		
		// Night Light (makeshift reflection)
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));

#pragma region Obstacles

		auto& obsA = world->CreateEntity({ -14.0f, 2.0f, -9.0f }, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 10.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsA, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsAMat = world->AddComponent<Frosty::ECS::CMaterial>(obsA, Frosty::AssetManager::GetShader("FlatColor"));
		obsAMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsA, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsB = world->CreateEntity({ 3.0f, 2.0f, -20.0f }, { 0.0f, 0.0f, 0.0f }, { 9.0f, 4.0f, 5.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsB, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsBMat = world->AddComponent<Frosty::ECS::CMaterial>(obsB, Frosty::AssetManager::GetShader("FlatColor"));
		obsBMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsB, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsC = world->CreateEntity({ -4.0f, 2.0f, 20.0f }, { 0.0f, 0.0f, 0.0f }, { 14.0f, 4.0f, 9.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsC, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsCMat = world->AddComponent<Frosty::ECS::CMaterial>(obsC, Frosty::AssetManager::GetShader("FlatColor"));
		obsCMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsC, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsD = world->CreateEntity({ 20.0f, 2.0f, 13.0f }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 4.0f, 14.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsD, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsDMat = world->AddComponent<Frosty::ECS::CMaterial>(obsD, Frosty::AssetManager::GetShader("FlatColor"));
		obsDMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsD, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsE = world->CreateEntity({ 2.0f, 2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 4.0f, 3.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsE, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsEMat = world->AddComponent<Frosty::ECS::CMaterial>(obsE, Frosty::AssetManager::GetShader("FlatColor"));
		obsEMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsE, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsF = world->CreateEntity({ 13.0f, 2.0f, -9.0f }, { 0.0f, 0.0f, 0.0f }, { 8.0f, 4.0f, 8.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsF, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsFMat = world->AddComponent<Frosty::ECS::CMaterial>(obsF, Frosty::AssetManager::GetShader("FlatColor"));
		obsFMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsF, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsG = world->CreateEntity({ -17.0f, 2.0f, 15.0f }, { 0.0f, 0.0f, 0.0f }, { 4.0f, 4.0f, 12.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsG, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsGMat = world->AddComponent<Frosty::ECS::CMaterial>(obsG, Frosty::AssetManager::GetShader("FlatColor"));
		obsGMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsG, Frosty::AssetManager::GetBoundingBox("pCube1"));

		auto& obsH = world->CreateEntity({ -8.0f, 2.0f, 8.0f }, { 0.0f, 0.0f, 0.0f }, { 8.0f, 4.0f, 5.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(obsH, Frosty::AssetManager::GetMesh("pCube1"));
		auto& obsHMat = world->AddComponent<Frosty::ECS::CMaterial>(obsH, Frosty::AssetManager::GetShader("FlatColor"));
		obsHMat.Albedo = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
		world->AddComponent<Frosty::ECS::CPhysics>(obsH, Frosty::AssetManager::GetBoundingBox("pCube1"));
		
#pragma endregion Obstacles

		navSystem->InitiateGridMap(planeTransform);

		auto& player = world->CreateEntity({ -19.0f, 0.0f, 27.0f }, { 0.0f,0.0f,0.0f }, { 2.0f, 2.0f, 2.0f });
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		//world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("pCylinder1"));
		//auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("FlatColor"));
		//playerMat.Albedo = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player);
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CWeapon>(player, Frosty::ECS::CWeapon::WeaponType::Arrow, 10.f);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		//auto& camEntity = world->GetSceneCamera();
		//world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		auto& enemyA = world->CreateEntity({ 25.0f, 2.0f, -27.0f }, { 0.0f,0.0f,0.0f }, { 1.0f, 2.0f, 1.0f });
		world->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("pCylinder1"));
		auto& enemyMatA = world->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("FlatColor"));
		enemyMatA.Albedo = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		world->AddComponent<Frosty::ECS::CEnemy>(enemyA, &playerTransform);
		world->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("pCylinder1"), 6.0f);

		//auto& enemyB = world->CreateEntity({ 17.0f, 2.0f, -17.0f }, { 0.0f,0.0f,0.0f }, { 1.0f, 2.0f, 1.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(enemyB, Frosty::AssetManager::GetMesh("pCylinder1"));
		//auto& enemyMatB = world->AddComponent<Frosty::ECS::CMaterial>(enemyB, Frosty::AssetManager::GetShader("FlatColor"));
		//enemyMatB.Albedo = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
		//world->AddComponent<Frosty::ECS::CEnemy>(enemyB, &playerTransform);
		//world->AddComponent<Frosty::ECS::CPhysics>(enemyB, Frosty::AssetManager::GetBoundingBox("pCylinder1"), 6.0f);

		//auto& player = world->CreateEntity({ -3.0f, 0.0f, -3.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		//auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		//world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("scarlet"));
		//auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Texture2D"));
		//playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		//playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		//playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		//world->AddComponent<Frosty::ECS::CPlayer>(player);
		//world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		//world->AddComponent<Frosty::ECS::CDash>(player);
		//world->AddComponent<Frosty::ECS::CWeapon>(player, Frosty::ECS::CWeapon::WeaponType::Arrow, 10.f);
		//world->AddComponent<Frosty::ECS::CHealth>(player);
		//world->AddComponent<Frosty::ECS::CInventory>(player);
		//world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		//auto& camEntity = world->GetSceneCamera();
		//world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		
//#ifdef FY_DEBUG || FY_RELEASE
		PushLayer(FY_NEW InspectorLayer());
//#else
//		Application::Get().StartGame(true);
//#endif // FY_DEBUG
	}


	Game::~Game()
	{
	}
}

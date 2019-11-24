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
		world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<AnimationSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<FollowSystem>();
		world->AddSystem<AttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
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
		particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));

		// SCENE 1
		// PLANE
		//auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 200.0f, 1.0f, 100.0f }, true);
		//auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
		//world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("FlatColor"));
		//planeMat.Albedo = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
		//world->AddToGroup(plane);
		
		// LIGHT 1
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.5f, 0.6f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -0.8, -1.0);

		// WEAPON 1
		auto& weapon = world->CreateEntity({ -0.7f, 2.f, 0.8f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
		Frosty::Weapon loadedWeapon = weaponHandler->GetWeaponByType(Frosty::Weapon::WeaponType::Sword);
		world->AddComponent<Frosty::ECS::CWeapon>(weapon, loadedWeapon, true);	
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);
		auto& weaponMesh = world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("sword2"));
		auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("Texture2D"));

		// PLAYER
		auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		//auto& player = world->CreateEntity({ -90.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("testSlash");
		auto& animation = world->GetComponent<Frosty::ECS::CAnimController>(player);
		animation.animSpeed = 0.7f;
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("ScarRun:model:scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player, &playerWeaponComp);	// <-- Give player a weapon
		//weaponTransform.Position += playerTransform.Position;//Check this
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 13.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player, 20);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		//Parent the weapon to player mesh.
		weaponMesh.parentMatrix = playerTransform.GetModelMatrix();
		//Make it move according to the player's hand.
		weaponMesh.animOffset = animation.currAnim->getHoldingJoint();
		//Update it in renderer.
		Frosty::Renderer::UpdateCMesh(weapon->Id, &weaponMesh);
	
		// TORCH
		auto& torch = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		world->AddComponent<Frosty::ECS::CLight>(torch, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), &playerTransform, glm::vec3(0.f, 5.f, 0.f));

		// ENEMY WEAPON 
		auto& enemyWeaponA = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		auto& enemyWeaponCompA = world->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.0f);
		enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
		//enemyWeaponCompA.MaxAttackRange = 5.0f;
		//enemyWeaponCompA.MinAttackRange = 0.0f;
		enemyWeaponCompA.MaxAttackRange = 22.0f;
		enemyWeaponCompA.MinAttackRange = 18.0f;
		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
		enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow
		
		/*
		// ENEMY A
		auto& enemyA = world->CreateEntity({ -45.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
		world->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Cult:Group43567"));
		auto& enemyMatA = world->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
		enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Cult_Diffuse");
		enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("Cult_defaultMat_Normal");
		world->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Cult:Group43567"), 6.0f);
		auto& enemyComp = world->AddComponent<Frosty::ECS::CEnemy>(enemyA, &playerTransform, &enemyWeaponCompA, 0.1f);
		enemyComp.SpawnPosition = { -45.0f, 0.0f, 0.0f };
		world->AddComponent<Frosty::ECS::CHealth>(enemyA, 10);
		world->AddComponent<Frosty::ECS::CHealthBar>(enemyA, glm::vec3(0.0f, 10.0f, 0.0f));
		world->AddComponent<Frosty::ECS::CDropItem>(enemyA);


		// TREE
		auto& tree = world->CreateEntity({ 50.0f, 3.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 8.0f, 6.0f, 7.0f }, true);
		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("pCube1"));
		world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("pCube1"));

		navSystem->InitiateGridMap(planeTransform);*/

		// TEXT
		auto& GUI = world->CreateEntity();
		Frosty::UILayout uiLayout(3, 1);
		uiLayout.AddText(glm::vec2(25.0f, 20.0f), "Hello team");
		uiLayout.AddText(glm::vec2(20.0f, 700.0f), "uwu", glm::vec3(1.0f, 0.0f, 1.0f), 0.25f);
		uiLayout.AddText(glm::vec2(25.0f, 220.0f), "1234!", glm::vec3(0.5f, 0.1f, 0.9f), 1.5f);
		
		world->AddComponent<Frosty::ECS::CGUI>(GUI, uiLayout);

		//Particle system example
		//The constructor is super-important!! Make sure that you don't use an empty constructor. Most variables aren't supposed to be changed after initialization
		//auto& ParticleSystem2 = world->CreateEntity();
		//auto& particleSystem2Transform = world->GetComponent<Frosty::ECS::CTransform>(ParticleSystem2);
		//particleSystem2Transform.Position = glm::vec3(0.0f, 10.0f, 0.0f);
		//auto& particleSystemComp2 = world->AddComponent<Frosty::ECS::CParticleSystem>(ParticleSystem2, "ParticlesHorizontal", "particleFancy", 50, glm::vec3(1.0f, 0.0f, 0.0f), 4.0f);
		//particleSystemComp2.RandomDirection = true;
		//particleSystemComp2.StaticColor = false;
		//particleSystemComp2.ParticleSystemDirection = glm::vec3(0.0f, -1.0f, 0.0f);
		//particleSystemComp2.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
		//particleSystemComp2.StartParticleSize = 5.0f;
		//particleSystemComp2.EndParticleSize = 2.0f;

//ifdef FY_DEBUG
	PushLayer(FY_NEW InspectorLayer());
//else
		//Application::Get().StartGame(true);
//endif // FY_DEBUG
	}

	Game::~Game()
	{
	}
}
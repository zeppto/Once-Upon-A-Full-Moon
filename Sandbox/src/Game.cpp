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
		world->AddSystem<AnimationSystem>();
		world->AddSystem<HealthBarSystem>();
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
		// LIGHT 1
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.6f, 0.7f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -1.0, 1.0);
		//LIGHT 2
		auto& light2 = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { -190.0f, 8.0f, -10.0f });
		auto& DLight2 = world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Directional, 0.3f, glm::vec3(0.6f, 0.7f, 1.f));
		DLight2.Direction = glm::vec3(-1.0f, -1.0, 1.0);

		// WEAPON 1
		auto& weapon = world->CreateEntity({ 0.f, 0.f, 0.f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
		//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
		world->AddComponent<Frosty::ECS::CWeapon>(weapon, Frosty::ECS::CWeapon::WeaponType::Arrow, 3, 10.f);
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);

		// PLAYER
		//auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& player = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("NewRun");
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("ScarRun:model:scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);	// <-- Give player a weapon
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("scarlet"), 10.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player);
		world->AddComponent<Frosty::ECS::CInventory>(player);
		world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		// TEXT
		auto& GUI = world->CreateEntity();
		Frosty::UILayout uiLayout(3, 1);
		uiLayout.AddText(glm::vec2(25.0f, 20.0f), "Hello team");
		uiLayout.AddText(glm::vec2(20.0f, 700.0f), "uwu", glm::vec3(1.0f, 0.0f, 1.0f), 0.25f);
		uiLayout.AddText(glm::vec2(25.0f, 220.0f), "1234!", glm::vec3(0.5f, 0.1f, 0.9f), 1.5f);
		
		world->AddComponent<Frosty::ECS::CGUI>(GUI, uiLayout);

		//Particle System Test
		auto& ParticleSystem1 = world->CreateEntity();
		auto& particleSystem1Transform = world->GetComponent<Frosty::ECS::CTransform>(ParticleSystem1);
		world->AddComponent<Frosty::ECS::CParticleSystem>(ParticleSystem1);

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

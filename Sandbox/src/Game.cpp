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

//#include "States/LoadingState.hpp"
#include "States/MenuState.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& state = Application::Get().GetStateMachine();
		//state.AddState(Frosty::StateRef(FY_NEW(LoadingState)));
		state.AddState(Frosty::StateRef(FY_NEW(MenuState)));
		Application::Get().StartGame(true);

//		auto& world = Application::Get().GetWorld();
//		// Add systems
//		world->AddSystem<LevelSystem>();
//		world->AddSystem<CameraSystem>();
//		world->AddSystem<LightSystem>();
//		world->AddSystem<AnimationSystem>();
//		world->AddSystem<RenderSystem>();
//		world->AddSystem<PlayerControllerSystem>();
//		world->AddSystem<PhysicsSystem>();
//		world->AddSystem<FollowSystem>();
//		world->AddSystem<AttackSystem>();
//		world->AddSystem<CombatSystem>();
//		world->AddSystem<DestroySystem>();
//		world->AddSystem<HealthBarSystem>();
//		world->AddSystem<AISystem>();
//		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
//		NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
//		retSystem = world->AddSystem<ParticleSystem>();
//
//		ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);
//
//		world->AddSystem<BossBehaviorSystem>();
//		world->AddSystem<GUISystem>();
//		world->AddSystem<LootingSystem>();
//
//		world->Awake();
//		particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));
//
//		// SCENE 1
//		// PLANE
//		//auto& plane = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 200.0f, 1.0f, 100.0f }, true);
//		//auto& planeTransform = world->GetComponent<Frosty::ECS::CTransform>(plane);
//		//world->AddComponent<Frosty::ECS::CMesh>(plane, Frosty::AssetManager::GetMesh("pPlane1"));
//		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(plane, Frosty::AssetManager::GetShader("FlatColor"));
//		//planeMat.Albedo = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
//		//world->AddToGroup(plane);
//		
//		// LIGHT 1
//		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
//		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.5f, 0.6f, 1.f));
//		DLight.Direction = glm::vec3(-1.0f, -0.8, -1.0);
//
//		// WEAPON 1
//		//Sword Offset
//		auto& weapon = world->CreateEntity({ -0.7f, 2.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
//		//Bow Offset
//		/*auto& weapon = world->CreateEntity({ -0.7f, 2.3f, 0.2f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });*/
//		auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
//		Frosty::Weapon loadedWeapon = weaponHandler->GetWeaponByType(Frosty::Weapon::WeaponType::Sword);
//		world->AddComponent<Frosty::ECS::CWeapon>(weapon, loadedWeapon, true);	
//		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);
//		weaponComp.Level = 3;
//		auto& weaponMesh = world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("sword"));
//		auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
//
//
//
//		// PLAYER
//		auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
//		//auto& player = world->CreateEntity({ -90.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
//		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
//		world->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
//		auto& animation = world->GetComponent<Frosty::ECS::CAnimController>(player);
//		animation.animSpeed = 0.7f;
//		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Scarlet"));
//		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
//		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
//		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
//		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
//		world->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);	// <-- Give player a weapon
//		//weaponTransform.Position += playerTransform.Position;//Check this
//		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("Scarlet"), 13.0f);
//		world->AddComponent<Frosty::ECS::CDash>(player);
//		world->AddComponent<Frosty::ECS::CHealth>(player, 20);
//		world->AddComponent<Frosty::ECS::CInventory>(player);
//		world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
//		auto& camEntity = world->GetSceneCamera();
//		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;
//
//		//Parent the weapon to player mesh.
//		weaponMesh.parentMatrix = playerTransform.GetModelMatrix();
//		animation.holdPtr = animation.currAnim->getHoldingJoint();
//		//Make it move according to the player's hand.
//		weaponMesh.animOffset = animation.holdPtr;
//		//Update it in renderer.
//		Frosty::Renderer::UpdateCMesh(weapon->Id, &weaponMesh);
//	
//		// TORCH
//		auto& torch = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
//		world->AddComponent<Frosty::ECS::CLight>(torch, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), &playerTransform, glm::vec3(0.f, 5.f, 0.f));
//		//Player HUD
//		Frosty::UILayout uiLayout(21, 14);
//		 
//		//Items
//		float padding = 200.0f;
//		float offsetX = 700.0f;
//		float offsetY = 30.0f;
//
//		//Sprites
//		int itemSpriteXOffset = 920;
//		int itemSpriteYOffset = 40;
//		int itemSpritePadding = 110;
//
//		glm::vec2 hpPotionSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 0, itemSpriteYOffset);
//		glm::vec2 spPotionSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 1, itemSpriteYOffset);
//		glm::vec2 baitSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 2, itemSpriteYOffset);
//		glm::vec2 wolfsbainSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 3, itemSpriteYOffset);
//
//		//Amount info
//		int itemNrOfXOffset = itemSpriteXOffset - 60;
//
//		glm::vec2 hpPotionNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 0, 30.0f);
//		glm::vec2 spPotionNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 1, 30.0f);
//		glm::vec2 baitNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 2, 30.0f);
//		glm::vec2 wolfsbainNrOf = glm::vec2(itemNrOfXOffset + 10 + itemSpritePadding * 3, 30.0f);
//
//		//Cooldown
//		int itemCoolDownXOffset = itemSpriteXOffset - 15;
//		int itemCoolDownYOffset = itemSpriteYOffset - 10;
//
//		glm::vec2 hpPotionCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 0, itemCoolDownYOffset);
//		glm::vec2 spPotionCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 1, itemCoolDownYOffset);
//		glm::vec2 baitCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 2, itemCoolDownYOffset);
//
//		//Controls
//		int itemControllXOffset = itemCoolDownXOffset + 8;
//		int itemControllYOffset = itemSpriteYOffset + 40;
//
//		glm::vec2 hpPotionControl = glm::vec2(itemControllXOffset + itemSpritePadding * 0, itemControllYOffset);
//		glm::vec2 spPotionControl = glm::vec2(itemControllXOffset + itemSpritePadding * 1, itemControllYOffset);
//		glm::vec2 baitControl = glm::vec2(itemControllXOffset + itemSpritePadding * 2, itemControllYOffset);
//
//
//		uiLayout.AddText(hpPotionNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //0
//		uiLayout.AddText(spPotionNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //1
//		uiLayout.AddText(baitNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //2
//		uiLayout.AddText(wolfsbainNrOf, "1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //3
//
//		//Points
//		uiLayout.AddText(glm::vec2(1100, 675), "Points: 100", glm::vec3(1.0f, 1.0f, 0.75f), 0.75f); //4
//
//		//TempHealth
//		uiLayout.AddText(glm::vec2(25, 675), "100/100", glm::vec3(1.0f, 1.0f, 0.75f)); //5
//
//		//Picked up
//		//uiLayout.AddText(glm::vec2(550, 425), "+ 1 Health Potion", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f);
//		uiLayout.AddText(glm::vec2(550, 425), "", glm::vec3(1.0f, 1.0f, 0.75f), 0.75f); //6
//
//		//Attack cooldown
//		uiLayout.AddText(glm::vec2(35, 134), "3.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //7
//		uiLayout.AddText(glm::vec2(90, 105), "2.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //8
//		uiLayout.AddText(glm::vec2(115, 45), "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //9
//
//		//Dash cooldown
//		uiLayout.AddText(glm::vec2(200, 40), "0.0", glm::vec3(1.0f, 1.0f, 1.0f),0.65f); //10
//
//		//Item cooldown
//		float offsetx2 = 50;
//		uiLayout.AddText(hpPotionCooldown, "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //11
//		uiLayout.AddText(spPotionCooldown, "2.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //12
//		uiLayout.AddText(baitCooldown, "3.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //13
//
//		//Controls info
//		float controlsInfoSize = 0.5f; 
//		float controlsInfoSize2 = 0.30f;
//		//Attacks															
//		uiLayout.AddText(glm::vec2(30, 150 +15), "[SPACE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //14
//		uiLayout.AddText(glm::vec2(80, 100 + 35), "[R-MOUSE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //15
//		uiLayout.AddText(glm::vec2(105, 40 + 35), "[L-MOUSE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //16
//
//		//Dash
//		uiLayout.AddText(glm::vec2(240, 40 + 35), "[SHIFT]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //17
//
//		//Items
//		uiLayout.AddText(hpPotionControl, "[1]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //18
//		uiLayout.AddText(spPotionControl, "[2]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //19
//		uiLayout.AddText(baitControl, "[Q]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //20
//		
//
//
//		uiLayout.AddSprite(glm::vec2(960 / 1.5f, 540 / 1.5f), glm::vec2(16 * 1.6, 9 * 1.6f), "fearEffect", glm::vec4(0.0f, 0.0f, 0.0f, 0.75f)); // 0
//
//		//Weapon
//		uiLayout.AddSprite(glm::vec2(55.0f, 75.0f), glm::vec2(1, 1), "attackRanged", glm::vec4(1.0f));// 1
//
//		glm::vec2 attackScale = glm::vec2(0.75, 0.75);
//
//		//Unlocked = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
//		//Cooldown =  glm::vec4(0.1f, 0.1f, 0.1f, 0.90f)
//		//Locked glm::vec4(0.1f, 0.1f, 0.1f, 0.50f)
//
//		uiLayout.AddSprite(glm::vec2(130 , 50.0f), attackScale, "attackRanged1", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 2
//		uiLayout.AddSprite(glm::vec2(105 , 110.0f), attackScale, "attackRanged2", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 3
//		uiLayout.AddSprite(glm::vec2(50.0f  , 140.0f), attackScale, "attackRanged3", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 4
//
//		int elementXOffset = 30;
//		int elementyOffset = 30;
//		int elementPadding = 20;
//
//		glm::vec2 elementScale = glm::vec2(0.5, 0.5);
//
//		//Element
//		//Normal
//		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 0, elementyOffset), elementScale, "elementEarth", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 5
//		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 1, elementyOffset), elementScale, "elementFire", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 6
//		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 2, elementyOffset), elementScale, "elementWater", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 7
//		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 3, elementyOffset), elementScale, "elementWind", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 8
//
//		//Empty
//	//	uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 0, elementyOffset), elementScale, "elementEarthEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 5
//		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 1, elementyOffset), elementScale, "elementFireEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 6
//		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 2, elementyOffset), elementScale, "elementWaterEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 7
//		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 3, elementyOffset), elementScale, "elementWindEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 8
//		
//		
//
//		uiLayout.AddSprite(hpPotionSprite, glm::vec2(1, 1), "hpPotion", glm::vec4(1.0f));// 9
//		uiLayout.AddSprite(spPotionSprite, glm::vec2(1, 1), "spPotion", glm::vec4(1.0f));// 10
//		uiLayout.AddSprite(baitSprite, glm::vec2(1, 1), "bait", glm::vec4(1.0f));// 11
//		uiLayout.AddSprite(wolfsbainSprite, glm::vec2(1, 1), "wolfsbain", glm::vec4(1.0f));// 12
//
//		////Need to change this sprite to a "dodge" sprite ////
//		uiLayout.AddSprite(glm::vec2(215, 45), glm::vec2(1, 1), "attackRanged3", glm::vec4(1.0f));// 13
//	
//
//
//		//uiLayout.AddSprite(glm::vec2(25.0f + testOffset * 0, 620.0f), glm::vec2(1, 1), "higlightHart", glm::vec4(1.0f));
//		world->AddComponent<Frosty::ECS::CGUI>(player, uiLayout);
//
//
//
//
//
//
//		//// WALL
//		//auto& wall = world->CreateEntity({ -16.0f, 5.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 15.0f, 10.0f, 20.0f }, true);
//		//world->AddComponent<Frosty::ECS::CMesh>(wall, Frosty::AssetManager::GetMesh("pCube1"));
//		//world->AddComponent<Frosty::ECS::CMaterial>(wall, Frosty::AssetManager::GetShader("FlatColor"));
//		//world->AddComponent<Frosty::ECS::CPhysics>(wall, Frosty::AssetManager::GetBoundingBox("pCube1"));
//
//		// ENEMY WEAPON 
//		auto& enemyWeaponA = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f });
//		//world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("pCube1"));
//		//world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));
//		auto& enemyWeaponCompA = world->AddComponent<Frosty::ECS::CWeapon>(enemyWeaponA, Frosty::ECS::CWeapon::WeaponType::Bow, 1, 1.0f);
//		enemyWeaponCompA.LVL1AttackCooldown = 3.0f;
//		//enemyWeaponCompA.MaxAttackRange = 5.0f;
//		//enemyWeaponCompA.MinAttackRange = 0.0f;
//		enemyWeaponCompA.MaxAttackRange = 22.0f;
//		enemyWeaponCompA.MinAttackRange = 18.0f;
//		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(10.0f, 6.0f, 4.0f);				// Sword
//		//enemyWeaponCompA.AttackHitboxScale = glm::vec3(4.0f, 6.0f, 4.0f);				// Bite
//		enemyWeaponCompA.AttackHitboxScale = glm::vec3(0.3f);							// Arrow
//		
//		/*
//		// ENEMY A
//		auto& enemyA = world->CreateEntity({ -45.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
//		//world->AddComponent<Frosty::ECS::CAnimController>(enemyA).currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
//		world->AddComponent<Frosty::ECS::CMesh>(enemyA, Frosty::AssetManager::GetMesh("Cult:Group43567"));
//		auto& enemyMatA = world->AddComponent<Frosty::ECS::CMaterial>(enemyA, Frosty::AssetManager::GetShader("Texture2D"));
//		enemyMatA.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Cult_Diffuse");
//		enemyMatA.NormalTexture = Frosty::AssetManager::GetTexture2D("Cult_defaultMat_Normal");
//		world->AddComponent<Frosty::ECS::CPhysics>(enemyA, Frosty::AssetManager::GetBoundingBox("Cult:Group43567"), 6.0f);
//		auto& enemyComp = world->AddComponent<Frosty::ECS::CEnemy>(enemyA, &playerTransform, &enemyWeaponCompA, 0.1f);
//		enemyComp.SpawnPosition = { -45.0f, 0.0f, 0.0f };
//		world->AddComponent<Frosty::ECS::CHealth>(enemyA, 10);
//		world->AddComponent<Frosty::ECS::CHealthBar>(enemyA, glm::vec3(0.0f, 10.0f, 0.0f));
//		world->AddComponent<Frosty::ECS::CDropItem>(enemyA);
//
//
//		// TREE
//		auto& tree = world->CreateEntity({ 50.0f, 3.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 8.0f, 6.0f, 7.0f }, true);
//		world->AddComponent<Frosty::ECS::CMesh>(tree, Frosty::AssetManager::GetMesh("pCube1"));
//		world->AddComponent<Frosty::ECS::CMaterial>(tree, Frosty::AssetManager::GetShader("FlatColor"));
//		world->AddComponent<Frosty::ECS::CPhysics>(tree, Frosty::AssetManager::GetBoundingBox("pCube1"));
//
//		navSystem->InitiateGridMap(planeTransform);*/
//
//		
//		//navSystem->InitiateGridMap(world->GetComponent<Frosty::ECS::CTransform>(plane));
//		
//		//world->PrintWorld();
//
//		//Particle system example
//		//The constructor is super-important!! Make sure that you don't use an empty constructor. Most variables aren't supposed to be changed after initialization
//		//auto& ParticleSystem2 = world->CreateEntity();
//		//auto& particleSystem2Transform = world->GetComponent<Frosty::ECS::CTransform>(ParticleSystem2);
//		//particleSystem2Transform.Position = glm::vec3(0.0f, 10.0f, 0.0f);
//		//auto& particleSystemComp2 = world->AddComponent<Frosty::ECS::CParticleSystem>(ParticleSystem2, "ParticlesHorizontal", "particleFancy", 50, glm::vec3(1.0f, 0.0f, 0.0f), 4.0f);
//		//particleSystemComp2.RandomDirection = true;
//		//particleSystemComp2.StaticColor = false;
//		//particleSystemComp2.ParticleSystemDirection = glm::vec3(0.0f, -1.0f, 0.0f);
//		//particleSystemComp2.SystemEndColor = glm::vec3(0.0f, 0.0f, 1.0f);
//		//particleSystemComp2.StartParticleSize = 5.0f;
//		//particleSystemComp2.EndParticleSize = 2.0f;
//
////ifdef FY_DEBUG
//	PushLayer(FY_NEW InspectorLayer());
////else
//		//Application::Get().StartGame(true);
////endif // FY_DEBUG
	}

	Game::~Game()
	{
	}
}

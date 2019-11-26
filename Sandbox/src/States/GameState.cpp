#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/LightSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"

namespace MCS
{
	GameState::GameState()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_App = &Frosty::Application::Get();

		InitiateLight();
		SetPlayer();
	}

	GameState::~GameState()
	{
		// Get Player last position ...
		DeletePlayer();
		DeleteLight();
	}

	void GameState::Initiate()
	{
		//InitiateLight();
		//InitiateObjects();
		//InitiateHUD();
	}

	void GameState::OnInput()
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		{
			FY_INFO("GAME PAUSE STAGE NEXT");
			auto& world = Frosty::Application::Get().GetWorld();
			world->PauseGame();
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GamePauseState)));
		}
	}

	void GameState::OnUpdate()
	{
	}

	void GameState::InitiateLight()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_Light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(m_Light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.5f, 0.6f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -0.8, -1.0);
	}

	void GameState::InitiateObjects()
	{
	}

	void GameState::SetPlayer()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		//WEAPON 1
		//Sword Offset
		m_Weapon = world->CreateEntity({ -0.7f, 2.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		//Bow Offset
		/*auto& weapon = world->CreateEntity({ -0.7f, 2.3f, 0.2f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });*/
		auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
		Frosty::Weapon loadedWeapon = weaponHandler->GetWeaponByType(Frosty::Weapon::WeaponType::Sword);
		world->AddComponent<Frosty::ECS::CWeapon>(m_Weapon, loadedWeapon, true);
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(m_Weapon);
		weaponComp.Level = 3;
		auto& weaponMesh = world->AddComponent<Frosty::ECS::CMesh>(m_Weapon, Frosty::AssetManager::GetMesh("sword"));
		auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(m_Weapon, Frosty::AssetManager::GetShader("FlatColor"));	

		m_Player = world->CreateEntity(m_PlayerFirstPos, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		//auto& player = world->CreateEntity({ -90.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(m_Player);
		world->AddComponent<Frosty::ECS::CAnimController>(m_Player).currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
		auto& animation = world->GetComponent<Frosty::ECS::CAnimController>(m_Player);
		animation.animSpeed = 0.7f;
		world->AddComponent<Frosty::ECS::CMesh>(m_Player, Frosty::AssetManager::GetMesh("Scarlet"));
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(m_Player, Frosty::AssetManager::GetShader("Animation"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(m_Player, &weaponComp);	// <-- Give player a weapon
		//weaponTransform.Position += playerTransform.Position;//Check this
		world->AddComponent<Frosty::ECS::CPhysics>(m_Player, Frosty::AssetManager::GetBoundingBox("Scarlet"), 13.0f);
		world->AddComponent<Frosty::ECS::CDash>(m_Player);
		world->AddComponent<Frosty::ECS::CHealth>(m_Player, 20);
		world->AddComponent<Frosty::ECS::CInventory>(m_Player);
		world->AddComponent<Frosty::ECS::CHealthBar>(m_Player, glm::vec3(0.0f, 10.0f, 0.0f));
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		//Parent the weapon to player mesh.
		weaponMesh.parentMatrix = playerTransform.GetModelMatrix();
		animation.holdPtr = animation.currAnim->getHoldingJoint();
		//Make it move according to the player's hand.
		weaponMesh.animOffset = animation.holdPtr;
		//Update it in renderer.
		Frosty::Renderer::UpdateCMesh(m_Weapon->Id, &weaponMesh);

		// TORCH
		m_Torch = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		world->AddComponent<Frosty::ECS::CLight>(m_Torch, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), &playerTransform, glm::vec3(0.f, 5.f, 0.f));

		//Player HUD
		Frosty::UILayout uiLayout(21, 14);
		//UiLayout = Frosty::UILayout(21, 14);

		//Items
		float padding = 200.0f;
		float offsetX = 700.0f;
		float offsetY = 30.0f;

		//Sprites
		int itemSpriteXOffset = 920;
		int itemSpriteYOffset = 40;
		int itemSpritePadding = 110;

		glm::vec2 hpPotionSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 0, itemSpriteYOffset);
		glm::vec2 spPotionSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 1, itemSpriteYOffset);
		glm::vec2 baitSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 2, itemSpriteYOffset);
		glm::vec2 wolfsbainSprite = glm::vec2(itemSpriteXOffset + itemSpritePadding * 3, itemSpriteYOffset);

		//Amount info
		int itemNrOfXOffset = itemSpriteXOffset - 60;

		glm::vec2 hpPotionNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 0, 30.0f);
		glm::vec2 spPotionNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 1, 30.0f);
		glm::vec2 baitNrOf = glm::vec2(itemNrOfXOffset + itemSpritePadding * 2, 30.0f);
		glm::vec2 wolfsbainNrOf = glm::vec2(itemNrOfXOffset + 10 + itemSpritePadding * 3, 30.0f);

		//Cooldown
		int itemCoolDownXOffset = itemSpriteXOffset - 15;
		int itemCoolDownYOffset = itemSpriteYOffset - 10;

		glm::vec2 hpPotionCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 0, itemCoolDownYOffset);
		glm::vec2 spPotionCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 1, itemCoolDownYOffset);
		glm::vec2 baitCooldown = glm::vec2(itemCoolDownXOffset + itemSpritePadding * 2, itemCoolDownYOffset);

		//Controls
		int itemControllXOffset = itemCoolDownXOffset + 8;
		int itemControllYOffset = itemSpriteYOffset + 40;

		glm::vec2 hpPotionControl = glm::vec2(itemControllXOffset + itemSpritePadding * 0, itemControllYOffset);
		glm::vec2 spPotionControl = glm::vec2(itemControllXOffset + itemSpritePadding * 1, itemControllYOffset);
		glm::vec2 baitControl = glm::vec2(itemControllXOffset + itemSpritePadding * 2, itemControllYOffset);

		uiLayout.AddText(hpPotionNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //0
		uiLayout.AddText(spPotionNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //1
		uiLayout.AddText(baitNrOf, "1/1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //2
		uiLayout.AddText(wolfsbainNrOf, "1", glm::vec3(1.0f, 1.0f, 0.75f), 0.75); //3
		
		//Points
		uiLayout.AddText(glm::vec2(1100, 675), "Points: 100", glm::vec3(1.0f, 1.0f, 0.75f), 0.75f); //4

		//TempHealth
		uiLayout.AddText(glm::vec2(25, 675), "100/100", glm::vec3(1.0f, 1.0f, 0.75f)); //5

		//Picked up
		//uiLayout.AddText(glm::vec2(550, 425), "+ 1 Health Potion", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f);
		uiLayout.AddText(glm::vec2(550, 425), "", glm::vec3(1.0f, 1.0f, 0.75f), 0.75f); //6

		//Attack cooldown
		uiLayout.AddText(glm::vec2(35, 134), "3.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //7
		uiLayout.AddText(glm::vec2(90, 105), "2.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //8
		uiLayout.AddText(glm::vec2(115, 45), "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //9

		//Dash cooldown
		uiLayout.AddText(glm::vec2(200, 40), "0.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //10

		//Item cooldown
		float offsetx2 = 50;
		uiLayout.AddText(hpPotionCooldown, "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //11
		uiLayout.AddText(spPotionCooldown, "2.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //12
		uiLayout.AddText(baitCooldown, "3.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //13

		//Controls info
		float controlsInfoSize = 0.5f;
		float controlsInfoSize2 = 0.30f;
		//Attacks															
		uiLayout.AddText(glm::vec2(30, 150 + 15), "[SPACE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //14
		uiLayout.AddText(glm::vec2(80, 100 + 35), "[R-MOUSE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //15
		uiLayout.AddText(glm::vec2(105, 40 + 35), "[L-MOUSE]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //16

		//Dash
		uiLayout.AddText(glm::vec2(240, 40 + 35), "[SHIFT]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //17

		//Items
		uiLayout.AddText(hpPotionControl, "[1]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //18
		uiLayout.AddText(spPotionControl, "[2]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //19
		uiLayout.AddText(baitControl, "[Q]", glm::vec3(1.0f, 1.0f, 0.75f), controlsInfoSize2); //20

		uiLayout.AddSprite(glm::vec2(960 / 1.5f, 540 / 1.5f), glm::vec2(16 * 1.6, 9 * 1.6f), "fearEffect", glm::vec4(0.0f, 0.0f, 0.0f, 0.75f)); // 0

		//Weapon
		uiLayout.AddSprite(glm::vec2(55.0f, 75.0f), glm::vec2(1, 1), "attackRanged", glm::vec4(1.0f));// 1

		glm::vec2 attackScale = glm::vec2(0.75, 0.75);

		//Unlocked = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		//Cooldown =  glm::vec4(0.1f, 0.1f, 0.1f, 0.90f)
		//Locked glm::vec4(0.1f, 0.1f, 0.1f, 0.50f)

		uiLayout.AddSprite(glm::vec2(130, 50.0f), attackScale, "attackRanged1", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 2
		uiLayout.AddSprite(glm::vec2(105, 110.0f), attackScale, "attackRanged2", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 3
		uiLayout.AddSprite(glm::vec2(50.0f, 140.0f), attackScale, "attackRanged3", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 4

		int elementXOffset = 30;
		int elementyOffset = 30;
		int elementPadding = 20;

		glm::vec2 elementScale = glm::vec2(0.5, 0.5);

		//Element
		//Normal
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 0, elementyOffset), elementScale, "elementEarth", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 5
		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 1, elementyOffset), elementScale, "elementFire", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 6
		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 2, elementyOffset), elementScale, "elementWater", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 7
		//uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 3, elementyOffset), elementScale, "elementWind", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 8

		//Empty
		//	uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 0, elementyOffset), elementScale, "elementEarthEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 5
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 1, elementyOffset), elementScale, "elementFireEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 6
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 2, elementyOffset), elementScale, "elementWaterEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 7
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 3, elementyOffset), elementScale, "elementWindEmpty", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 8

		uiLayout.AddSprite(hpPotionSprite, glm::vec2(1, 1), "hpPotion", glm::vec4(1.0f));// 9
		uiLayout.AddSprite(spPotionSprite, glm::vec2(1, 1), "spPotion", glm::vec4(1.0f));// 10
		uiLayout.AddSprite(baitSprite, glm::vec2(1, 1), "bait", glm::vec4(1.0f));// 11
		uiLayout.AddSprite(wolfsbainSprite, glm::vec2(1, 1), "wolfsbain", glm::vec4(1.0f));// 12

		////Need to change this sprite to a "dodge" sprite ////
		uiLayout.AddSprite(glm::vec2(215, 45), glm::vec2(1, 1), "attackRanged3", glm::vec4(1.0f));// 13

		//uiLayout.AddSprite(glm::vec2(25.0f + testOffset * 0, 620.0f), glm::vec2(1, 1), "higlightHart", glm::vec4(1.0f));

		world->AddComponent<Frosty::ECS::CGUI>(m_Player, uiLayout);
	}

	void GameState::SetEnemies()
	{
	}

	void GameState::DeletePlayer()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_Player))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_Player);
		}
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_Weapon))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_Weapon);
		}
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_Torch))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_Torch);
		}
	}

	void GameState::DeleteLight()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_Light))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_Light);
		}
	}

	void GameState::DeleteEnemies()
	{
	}
}

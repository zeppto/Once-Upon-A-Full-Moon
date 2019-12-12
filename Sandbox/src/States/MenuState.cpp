#include "mcspch.hpp"
#include "States/MenuState.hpp"
#include "States/GameState.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"

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
#include "Systems/WitchCircleSystem.hpp"
#include "Systems/MediaSystem.h"

namespace MCS
{
	MenuState::MenuState()
	{
	}

	MenuState::~MenuState()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_MenuGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_MenuGui);
		}
		if (m_InstructionButtonsLoaded)
		{
			if (!world->HasComponent<Frosty::ECS::CDestroy>(m_InstructionGui))
			{
				world->AddComponent<Frosty::ECS::CDestroy>(m_InstructionGui);
			}
		}
	}

	void MenuState::Initiate()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_App = &Frosty::Application::Get();

		if (!m_App->MenuLoaded())
		{
			world->PauseGame();
			InitiateSystems();
			InitiateObjects();
			m_App->SetMenuLoad(true);
		}
		if (!m_ButtonsLoaded)
		{
			InitiateButtons();
			m_ButtonsLoaded = true;
		}
		m_App->SetGameLoad(false);
	}

	void MenuState::OnInput()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();

		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();

		if (!m_Controls)
		{
			if (x > (605.0f * width) && x < (670.0f * width) && y >(400.0f * height) && y < (440.0f * height))
			{
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{
					auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);
					menuGui.Layout.texts[1].SetColor(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

					m_Instructions = true;
					InitiateInstructions();
				}
			}
			else if (x > (570.0f * width) && x < (700.0f * width) && y >(335 * height) && y < (365 * height))
			{
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{
					auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);
					std::string Controls = "Controls";
					float posX = (960 / 1.5f) - (Controls.size() / 2) * 17;

					menuGui.Layout.texts[0].SetText("");
					menuGui.Layout.texts[1].SetText("");
					menuGui.Layout.texts[2].SetText("");
					menuGui.Layout.texts[3].SetText("Back");
					menuGui.Layout.texts[3].SetPosition(glm::vec2(posX, 25.0f));
					menuGui.Layout.sprites[0].SetImage("Controls");
					menuGui.Layout.sprites[0].SetColorSprite(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					m_ButtonsLoaded = false;
					m_Controls = true;
				}
			}
			else if (x > (605.0f * width) && x < (665.0f * width) && y >(260.0f * height) && y < (290 * height))
			{
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{
					Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
				}
			}
		}
		else if (m_Controls)
		{
			if (x > (575.0f * width) && x < (640.0f * width) && y >(25.0f * height) && y < (60.0f * height))
			{
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{
					auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);
					std::string Exit = "Exit";
					float posX = (960 / 1.5f) - (Exit.size() / 2) * 17;

					menuGui.Layout.texts[0].SetText("Main Menu");
					menuGui.Layout.texts[1].SetText("Play");
					menuGui.Layout.texts[2].SetText("Controls");
					menuGui.Layout.texts[3].SetText("Exit");
					menuGui.Layout.texts[3].SetPosition(glm::vec2(posX, 250.0f));
					menuGui.Layout.sprites[0].SetImage("Background");
					menuGui.Layout.sprites[0].SetColorSprite(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

					m_ButtonsLoaded = true;
					m_Controls = false;
				}
			}
			else if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
			{
				auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);
				std::string Exit = "Exit";
				float posX = (960 / 1.5f) - (Exit.size() / 2) * 17;

				menuGui.Layout.texts[0].SetText("Main Menu");
				menuGui.Layout.texts[1].SetText("Play");
				menuGui.Layout.texts[2].SetText("Controls");
				menuGui.Layout.texts[3].SetText("Exit");
				menuGui.Layout.texts[3].SetPosition(glm::vec2(posX, 250.0f));
				menuGui.Layout.sprites[0].SetImage("Background");
				menuGui.Layout.sprites[0].SetColorSprite(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

				m_ButtonsLoaded = true;
				m_Controls = false;
			}
		}

		if (m_Instructions)
		{
			if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
			{
				m_Instructions = false;
				world->PlayGame();
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)), true);

				Frosty::EventBus::GetEventBus()->Publish<Frosty::StopMediaEvent>(Frosty::StopMediaEvent());
				Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMusicEvent>(Frosty::PlayMusicEvent("assets/music/Atmo_Skog.mp3", 1.0f));
			}
		}
	}

	void MenuState::OnUpdate()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();

		if (m_ButtonsLoaded)
		{
			//FY_INFO("{0} : {1} | {2} : {3}", width, height, x, y);

			if (x > (605.0f * width) && x < (670.0f * width) && y > (400.0f * height) && y < (440.0f * height))
			{
				// Menu sounds (if color is not that below, play sound before it changes)
				if (menuGui.Layout.texts[1].GetColor() != glm::vec3(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)))
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("assets/sounds/ArrowHit1.wav", 1.0f, 0.0f, false, 0));

				menuGui.Layout.texts[1].SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > (570.0f * width) && x < (700.0f * width) && y > (335 * height) && y < (365 * height))
			{
				if (menuGui.Layout.texts[2].GetColor() != glm::vec3(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)))
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("assets/sounds/ArrowHit1.wav", 1.0f, 0.0f, false, 0));
				
				menuGui.Layout.texts[2].SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > (605.0f * width) && x < (665.0f * width) && y > (260.0f * height) && y < (290 * height))
			{
				if (menuGui.Layout.texts[3].GetColor() != glm::vec3(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)))
					Frosty::EventBus::GetEventBus()->Publish<Frosty::PlayMediaEvent>(Frosty::PlayMediaEvent("assets/sounds/ArrowHit1.wav", 1.0f, 0.0f, false, 0));
				
				menuGui.Layout.texts[3].SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			}
			else
			{
				menuGui.Layout.texts[1].SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				menuGui.Layout.texts[2].SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				menuGui.Layout.texts[3].SetColor(glm::vec4(0.8f, 0.0f, 0.0f, 0.0f));
			}
		}
		else
		{
			if (x > (575.0f * width) && x < (640.0f * width) && y >(25.0f * height) && y < (60.0f * height))
			{
				menuGui.Layout.texts[3].SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			}
			else
			{
				menuGui.Layout.texts[3].SetColor(glm::vec4(0.8f, 0.0f, 0.0f, 0.0f));
			}
		}
		
		if (m_InstructionButtonsLoaded && m_Instructions)
		{
			auto& world = Frosty::Application::Get().GetWorld();
			auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_MenuGui);

			menuGui.Layout.texts[0].SetText("");
			menuGui.Layout.texts[1].SetText("");
			menuGui.Layout.texts[2].SetText("");
			menuGui.Layout.texts[3].SetText("");
			m_ButtonsLoaded = false;
			m_Controls = false;
		}
		else if (!m_Instructions && m_InstructionButtonsLoaded)
		{
			auto& world = Frosty::Application::Get().GetWorld();
			auto& insGui = world->GetComponent<Frosty::ECS::CGUI>(m_InstructionGui);

			insGui.Layout.texts[0].SetText("");
			insGui.Layout.texts[1].SetText("");
		}
	}

	void MenuState::InitiateSystems()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		world->AddSystem<LevelSystem>();
		world->AddSystem<CameraSystem>();
		world->AddSystem<LightSystem>();
		world->AddSystem<AnimationSystem>();
		world->AddSystem<RenderSystem>();
		world->AddSystem<PlayerControllerSystem>();
		world->AddSystem<PhysicsSystem>();
		world->AddSystem<AttackSystem>();
		world->AddSystem<CombatSystem>();
		world->AddSystem<DestroySystem>();
		world->AddSystem<HealthBarSystem>();
		world->AddSystem<AISystem>();
		world->AddSystem<NavigationSystem>();

		Frosty::ECS::BaseSystem* retSystem = world->AddSystem<ParticleSystem>();
		ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);

		world->AddSystem<BossBehaviorSystem>();
		world->AddSystem<GUISystem>();
		world->AddSystem<LootingSystem>();
		world->AddSystem<WitchCircleSystem>();
		world->AddSystem<MediaSystem>();

		world->Awake();
		particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));
	}

	void MenuState::InitiateObjects()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		// WEAPON 1
		//Sword Offset
		auto& weapon = world->CreateEntity({ -0.7f, 2.1f, 0.80f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		//Bow Offset
		/*auto& weapon = world->CreateEntity({ -0.7f, 2.3f, 0.2f }, { 0.0f, 60.0f, 0.0f }, { 1.f, 1.f, 1.f });*/
		auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
		Frosty::Weapon loadedWeapon = weaponHandler->GetAPlayerWeapon(1, 1);
		world->AddComponent<Frosty::ECS::CWeapon>(weapon, loadedWeapon, true);
		auto& weaponComp = world->GetComponent<Frosty::ECS::CWeapon>(weapon);
		Frosty::ECS::CMesh* weaponMesh;

		if (weaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
		{
			/*world->GetComponent<Frosty::ECS::CTransform>*/
			weaponMesh = &world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("Bow"));
			auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("Texture2D"));
			weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("bow_lvl1_diffuse");
			weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("bow_normal");
		}
		else
		{
			weaponMesh = &world->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("sword"));
			auto& weaponMat = world->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("Texture2D"));
			weaponMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("sword_lvl1_diffuse");
			weaponMat.NormalTexture = Frosty::AssetManager::GetTexture2D("sword_normal");
		}

		// PLAYER
		auto& player = world->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f });
		auto& playerTransform = world->GetComponent<Frosty::ECS::CTransform>(player);
		world->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Scarlet"));
		world->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
		auto& animation = world->GetComponent<Frosty::ECS::CAnimController>(player);
		animation.animSpeed = 0.7f;
		auto& playerMat = world->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
		playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		world->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);	// <-- Give player a weapon
		//weaponTransform.Position += playerTransform.Position;//Check this
		world->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("Scarlet"), playerTransform.Scale, 13.0f);
		world->AddComponent<Frosty::ECS::CDash>(player);
		world->AddComponent<Frosty::ECS::CHealth>(player, 20);
		world->AddComponent<Frosty::ECS::CInventory>(player);

		//world->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		auto& camEntity = world->GetSceneCamera();
		world->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;

		//Parent the weapon to player mesh.
		weaponMesh->parentMatrix = playerTransform.GetModelMatrix();
		animation.holdPtr = animation.currAnim->getHoldingJoint();
		//Make it move according to the player's hand.
		weaponMesh->animOffset = animation.holdPtr;
		//Update it in renderer.
		Frosty::Renderer::UpdateCMesh((int)weapon->Id, weaponMesh);

		// TORCH
		auto& torch = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		world->AddComponent<Frosty::ECS::CLight>(torch, Frosty::ECS::CLight::LightType::Point, 0.5f, glm::vec3(0.99f, 0.9f, 0.8f), 15.f, &playerTransform, glm::vec3(0.f, 7.f, 0.f), true);

		//Player HUD
		Frosty::UILayout uiLayout(21, 30);

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
		//uiLayout.AddText(glm::vec2(25, 600), "100/100", glm::vec3(1.0f, 1.0f, 0.75f)); //5
		uiLayout.AddText(glm::vec2(25, 600), "", glm::vec3(1.0f, 1.0f, 0.75f)); //5

		//Picked up
		//uiLayout.AddText(glm::vec2(550, 425), "+ 1 Health Potion", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f);
		uiLayout.AddText(glm::vec2(550, 425), "", glm::vec3(1.0f, 1.0f, 0.75f), 0.75f); //6

		//Attack cooldown
		uiLayout.AddText(glm::vec2(35, 134), "", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //7
		uiLayout.AddText(glm::vec2(90, 105), "", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //8
		uiLayout.AddText(glm::vec2(115, 45), "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //9

		//Dash cooldown
		uiLayout.AddText(glm::vec2(200, 40), "0.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.65f); //10

		//Item cooldown
		float offsetx2 = 50;
		uiLayout.AddText(hpPotionCooldown, "1.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //11
		uiLayout.AddText(spPotionCooldown, "2.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.75f); //12
		uiLayout.AddText(baitCooldown, "3.0", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f); //13

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


		uiLayout.AddSprite(glm::vec2(1280.f / 2, 720.f / 2), glm::vec2(16 * 1.6, 9 * 1.6f), "fearEffect", glm::vec4(0.0f, 0.0f, 0.0f, 0.75f)); // 0
		uiLayout.AddSprite(glm::vec2(0.f, 0.f), glm::vec2(10, 10), "IndicatorEffect", glm::vec4(0.0f)); // 1 

		//Weapon
		glm::vec2 attackScale = glm::vec2(0.75, 0.75);

		if (weaponComp.Type == Frosty::ECS::CWeapon::WeaponType::Bow)
		{
			uiLayout.AddSprite(glm::vec2(55.0f, 75.0f), glm::vec2(1, 1), "attackRanged", glm::vec4(1.0f));// 2
			uiLayout.AddSprite(glm::vec2(130, 50.0f), attackScale, "attackRanged1", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 3
			uiLayout.AddSprite(glm::vec2(105, 110.0f), attackScale, "attackRanged2", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 4
			uiLayout.AddSprite(glm::vec2(50.0f, 140.0f), attackScale, "attackRanged3", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 5
		}
		else
		{
			uiLayout.AddSprite(glm::vec2(55.0f, 75.0f), glm::vec2(1, 1), "attackMelee", glm::vec4(1.0f));// 2
			uiLayout.AddSprite(glm::vec2(130, 50.0f), attackScale, "attackMelee1", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 3
			uiLayout.AddSprite(glm::vec2(105, 110.0f), attackScale, "attackMelee2", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 4
			uiLayout.AddSprite(glm::vec2(50.0f, 140.0f), attackScale, "attackMelee3", glm::vec4(0.1f, 0.1f, 0.1f, 0.50f));// 5
		}

		int elementXOffset = 30;
		int elementyOffset = 30;
		int elementPadding = 20;

		glm::vec2 elementScale = glm::vec2(0.5, 0.5);

		//Element
		//Normal
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 0, elementyOffset), elementScale, "elementEarth", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 6
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 1, elementyOffset), elementScale, "elementFire", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 7
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 2, elementyOffset), elementScale, "elementWater", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 8
		uiLayout.AddSprite(glm::vec2(elementXOffset + elementPadding * 3, elementyOffset), elementScale, "elementWind", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));// 9

		uiLayout.AddSprite(hpPotionSprite, glm::vec2(1, 1), "hpPotion", glm::vec4(1.0f));// 10
		uiLayout.AddSprite(spPotionSprite, glm::vec2(1, 1), "spPotion", glm::vec4(1.0f));// 11
		uiLayout.AddSprite(baitSprite, glm::vec2(1, 1), "bait", glm::vec4(1.0f));// 12
		uiLayout.AddSprite(wolfsbainSprite, glm::vec2(1, 1), "wolfsbane", glm::vec4(1.0f));// 13

		////Need to change this sprite to a "dodge" sprite ////
		uiLayout.AddSprite(glm::vec2(215, 45), glm::vec2(1, 1), "attackRanged3", glm::vec4(1.0f));// 14

		//Speed boot
		int speedBuffXOffset = 30;
		int speedBuffYOffset = 650;
		int speedBuffPadding = 20;
		glm::vec2 speedBuffScale = glm::vec2(0.75, 0.75);

		//Health
		int healthXOffset = 30;
		int healthYOffset = 680;
		int healthPadding = 45;
		glm::vec2 healthScale = glm::vec2(0.75, 0.75);

		uiLayout.AddSprite(glm::vec2(speedBuffXOffset + speedBuffPadding * 0, speedBuffYOffset), speedBuffScale, "speedBoots", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 15
		uiLayout.AddSprite(glm::vec2(speedBuffXOffset + speedBuffPadding * 1, speedBuffYOffset), speedBuffScale, "speedBoots", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 16
		uiLayout.AddSprite(glm::vec2(speedBuffXOffset + speedBuffPadding * 2, speedBuffYOffset), speedBuffScale, "speedBoots", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 17
		uiLayout.AddSprite(glm::vec2(speedBuffXOffset + speedBuffPadding * 3, speedBuffYOffset), speedBuffScale, "speedBoots", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 18
		uiLayout.AddSprite(glm::vec2(speedBuffXOffset + speedBuffPadding * 4, speedBuffYOffset), speedBuffScale, "speedBoots", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 19


		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 0, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 20
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 1, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 21
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 2, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 22
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 3, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 23
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 4, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 24
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 5, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 25
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 6, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 26
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 7, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 27
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 8, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 28
		uiLayout.AddSprite(glm::vec2(healthXOffset + healthPadding * 9, healthYOffset), healthScale, "Heart_0", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f));// 29

		//uiLayout.AddSprite(glm::vec2(25.0f + testOffset * 0, 620.0f), glm::vec2(1, 1), "higlightHart", glm::vec4(1.0f));
		world->AddComponent<Frosty::ECS::CGUI>(player, uiLayout);

		////					<<<		FORWARD PLUS TESTING	>>>		plz don't touch		~ W-_-W ~
		//// LIGHTS
		//auto& light = world->CreateEntity({ 0.0f, 0.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		//world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), 20.f);		

		//auto& light2 = world->CreateEntity({ 0.0f, 0.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		//world->AddComponent<Frosty::ECS::CLight>(light2, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), 15.f);
		//
		//auto& light3 = world->CreateEntity({ 10.0f, 0.1f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		//world->AddComponent<Frosty::ECS::CLight>(light3, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), 9.f);

		////Test Bool Map
		//auto& planeBmap = world->CreateEntity({ 0.0f, 0.1f, 0.0f }, { 0.f,-90.0f, 0.0f }, { 300.0f, 1.0f, 300.0f });
		//world->AddComponent<Frosty::ECS::CMesh>(planeBmap, Frosty::AssetManager::GetMesh("pPlane1"));
		//auto& planeMat = world->AddComponent<Frosty::ECS::CMaterial>(planeBmap, Frosty::AssetManager::GetShader("Texture2D"));
		//planeMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("deadend_chests_IsStatick_t_p_e_r_h");

		//auto& light8 = world->CreateEntity({ -20.0f, 1.0f, 20.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		//world->AddComponent<Frosty::ECS::CLight>(light8, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), 20.f);

		//auto& light9 = world->CreateEntity({ -15.0f, 1.0f, -15.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		//world->AddComponent<Frosty::ECS::CLight>(light9, Frosty::ECS::CLight::LightType::Point, 1.f, glm::vec3(0.99f, 0.9f, 0.8f), 5.f);

		//// QUAD
		//auto& quad = world->CreateEntity({ 0.f, 0.05f, 0.f }, { 0.f, 0.f, 0.f }, { 1000.f, 1.f, 1000.f });
		//auto& quadTransform = world->GetComponent<Frosty::ECS::CTransform>(quad);
		//world->AddComponent<Frosty::ECS::CMesh>(quad, Frosty::AssetManager::GetMesh("pPlane1"));
		//auto& quadMat = world->AddComponent<Frosty::ECS::CMaterial>(quad, Frosty::AssetManager::GetShader("HeatMap"));
		//quadMat.Albedo = glm::vec4(0.f, 0.f, 1.f, 1.f);
	}

	void MenuState::InitiateButtons()
	{
		m_App = &Frosty::Application::Get();
		auto& world = Frosty::Application::Get().GetWorld();
		m_MenuGui = m_App->Get().GetWorld()->CreateEntity();
		
		m_UILayout = Frosty::UILayout(4, 1);
		std::string MainMenu = "Main Menu";
		std::string Play = "Play";
		std::string Controls = "Controls";
		std::string Exit = "Exit";

		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();
		float middleX = float(m_App->GetWindow().GetWidth() / 2);
		float middleY = float(m_App->GetWindow().GetHeight() / 2);
		
		float posX1 = (middleX / width) - (MainMenu.size() / 2) * 32;
		float posX2 = (middleX / width) - (Play.size()     / 2) * 17;
		float posX3 = (middleX / width) - (Controls.size() / 2) * 17;
		float posX4 = (middleX / width) - (Exit.size()     / 2) * 17;

		m_UILayout.AddText(glm::vec2(posX1, middleY / height + 175.0f), MainMenu, glm::vec3(1.0f, 1.0f, 0.0f), 1.5f);
		m_UILayout.AddText(glm::vec2(posX2, (middleY / height) + 50.0f), Play, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX3, (middleY / height) - 25.0f), Controls, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX4, (middleY / height) - 100.0f), Exit, glm::vec3(0.8f, 0.0f, 0.0f), 1.0f);
		m_UILayout.AddSprite(glm::vec2(640.0f, 360.0f), glm::vec2(25.6f, 14.4f), "Background", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

		world->AddComponent<Frosty::ECS::CGUI>(m_MenuGui, m_UILayout);
	}
	
	void MenuState::InitiateInstructions()
	{
		m_App = &Frosty::Application::Get();
		auto& world = Frosty::Application::Get().GetWorld();
		m_InstructionGui = m_App->Get().GetWorld()->CreateEntity();

		m_UILayout2 = Frosty::UILayout(2, 0);
		std::string Instruction = "Instructions";
		std::string Continue = "Press Space to Continue";

		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();
		float middleX = float(m_App->GetWindow().GetWidth() / 2);
		float middleY = float(m_App->GetWindow().GetHeight() / 2);

		float posX1 = (middleX / width) - (Instruction.size() / 2) * 32;
		float posX2 = (middleX / width) - (Continue.size()    / 2) * 15;

		m_UILayout2.AddText(glm::vec2(posX1, middleY / height + 175.0f), Instruction, glm::vec3(1.0f, 1.0f, 0.0f), 1.5f);
		m_UILayout2.AddText(glm::vec2(posX2, middleY / height + 100.0f), Continue, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		world->AddComponent<Frosty::ECS::CGUI>(m_InstructionGui, m_UILayout2);
		m_InstructionButtonsLoaded = true;
	}
}

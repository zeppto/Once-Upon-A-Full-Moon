#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/ParticleSystem.hpp"

namespace MCS
{
	GameState::GameState()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		m_App = &Frosty::Application::Get();

		if (!m_App->GetGameLoad())
		{
			m_World->AddSystem<LevelSystem>();





			InitiateLight();
			m_App->SetGameLoad(true);
		}
	}

	void GameState::Initiate()
	{
		//InitiateLight();
		InitiateObjects();
	}

	void GameState::OnInput()
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		{
			/*if (m_App->GameIsRunning())
			{
				m_App->StopGame(true);
			}
			else if (!m_App->GameIsRunning())
			{
				m_App->StartGame(true);
			}
			m_App->GetStateMachine().AddState(Frosty::StateRef(new GamePauseState()));*/

			FY_INFO("GAME PAUSE STAGE NEXT");
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GamePauseState)));
		}
	}

	void GameState::OnUpdate()
	{
	}

	void GameState::InitiateLight()
	{
		// LIGHT 1
		auto& light = m_World->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = m_World->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.5f, 0.6f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -0.8, -1.0);
	}

	void GameState::InitiateObjects()
	{
		//// WEAPON 1
		////Sword Offset
		//auto& weapon = m_World->CreateEntity({ -0.7f, 2.1f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });
		////Bow Offset
		///*auto& weapon = world->CreateEntity({ -0.7f, 2.3f, 0.2f }, { 0.0f, 0.0f, 0.0f }, { 1.f, 1.f, 1.f });*/
		//auto& weaponHandler = Frosty::AssetManager::GetWeaponHandler("Weapons");
		//Frosty::Weapon loadedWeapon = weaponHandler->GetWeaponByType(Frosty::Weapon::WeaponType::Sword);
		//m_World->AddComponent<Frosty::ECS::CWeapon>(weapon, loadedWeapon, true);
		//auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(weapon);
		//weaponComp.Level = 3;
		//auto& weaponMesh = m_World->AddComponent<Frosty::ECS::CMesh>(weapon, Frosty::AssetManager::GetMesh("sword"));
		//auto& weaponMat = m_World->AddComponent<Frosty::ECS::CMaterial>(weapon, Frosty::AssetManager::GetShader("FlatColor"));

		// PLAYER
		//auto& player = m_World->CreateEntity({ -104.0f, 0.0f, -15.4f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		////auto& player = world->CreateEntity({ -90.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 2.0f, 2.0f } );
		//auto& playerTransform = m_World->GetComponent<Frosty::ECS::CTransform>(player);
		//m_World->AddComponent<Frosty::ECS::CAnimController>(player).currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
		//auto& animation = m_World->GetComponent<Frosty::ECS::CAnimController>(player);
		//animation.animSpeed = 0.7f;
		//m_World->AddComponent<Frosty::ECS::CMesh>(player, Frosty::AssetManager::GetMesh("Scarlet"));
		//auto& playerMat = m_World->AddComponent<Frosty::ECS::CMaterial>(player, Frosty::AssetManager::GetShader("Animation"));
		//playerMat.DiffuseTexture = Frosty::AssetManager::GetTexture2D("Scarlet_diffuse");
		//playerMat.NormalTexture = Frosty::AssetManager::GetTexture2D("Scarlet_normal");
		//playerMat.SpecularTexture = Frosty::AssetManager::GetTexture2D("Scarlet_specular");
		//m_World->AddComponent<Frosty::ECS::CPlayer>(player, &weaponComp);	// <-- Give player a weapon
		////weaponTransform.Position += playerTransform.Position;//Check this
		//m_World->AddComponent<Frosty::ECS::CPhysics>(player, Frosty::AssetManager::GetBoundingBox("Scarlet"), 13.0f);
		//m_World->AddComponent<Frosty::ECS::CDash>(player);
		//m_World->AddComponent<Frosty::ECS::CHealth>(player, 20);
		//m_World->AddComponent<Frosty::ECS::CInventory>(player);
		//m_World->AddComponent<Frosty::ECS::CHealthBar>(player, glm::vec3(0.0f, 10.0f, 0.0f));
		//auto& camEntity = m_World->GetSceneCamera();
		//m_World->GetComponent<Frosty::ECS::CCamera>(camEntity).Target = &playerTransform;
	}
}

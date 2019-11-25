#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"
#include "Systems/LevelSystem.hpp"

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

	GameState::~GameState()
	{
		if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_GameGui))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(m_GameGui);
		}
	}

	void GameState::Initiate()
	{
		InitiateGui();
		//InitiateLight();
		//InitiateObjects();
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

	void GameState::InitiateGui()
	{
		m_GameGui = m_World->CreateEntity();

		Frosty::UILayout UILayout(1, 0);
		UILayout.AddText(glm::vec2(25.0f, 220.0f), "GameState", glm::vec3(0.0f, 1.0f, 0.0f), 1.5f);
		m_World->AddComponent<Frosty::ECS::CGUI>(m_GameGui, UILayout);
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
	}
}

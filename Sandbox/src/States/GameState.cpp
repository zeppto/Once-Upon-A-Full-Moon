#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
//#include "GamePauseState.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"
#include "Systems/LevelSystem.hpp"

namespace MCS
{
	GameState::GameState()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		m_World->AddSystem<LevelSystem>();
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
			FY_INFO("GAME PAUSE STAGE NEXT");
			//m_App->GetStateMachine().AddState(Frosty::StateRef(new GamePauseState()));
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
	}

	void GameState::InitiateObjects()
	{
	}
}

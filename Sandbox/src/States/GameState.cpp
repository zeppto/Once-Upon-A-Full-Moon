#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "GameOverState.hpp"
#include "GameWinState.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/LightSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"

#include <fstream>

namespace MCS
{
	GameState::GameState()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_App = &Frosty::Application::Get();
		Frosty::EventBus::GetEventBus()->Subscribe<GameState, Frosty::BaseEvent>(this, &GameState::OnEvent);
		world->PlayGame();
	}

	void GameState::Initiate()
	{
		if (!m_App->GameLoaded())
		{
			InitiateLight();
			m_App->SetGameLoad(true);
		}
	}

	void GameState::OnInput()
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		{
			auto& world = Frosty::Application::Get().GetWorld();
			world->PauseGame();
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GamePauseState)), true);
		}
	}

	void GameState::OnUpdate()
	{
	}

	void GameState::OnEvent(Frosty::BaseEvent & e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::GameOver:
			OnGameOverEvent(static_cast<Frosty::GameoverEvent&>(e));
			break;
		case Frosty::EventType::Win:
			OnGameWinEvent(static_cast<Frosty::WinEvent&>(e));
			break;
		}
	}

		void GameState::InitiateLight()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& light = world->CreateEntity({ 0.0f, 0.0f, 0.0f }, { 120.0f, 8.0f, -10.0f });
		auto& DLight = world->AddComponent<Frosty::ECS::CLight>(light, Frosty::ECS::CLight::LightType::Directional, 0.9f, glm::vec3(0.5f, 0.6f, 1.f));
		DLight.Direction = glm::vec3(-1.0f, -0.8, -1.0);
	}

	void GameState::OnGameOverEvent(Frosty::GameoverEvent& e)
	{
		auto& world = Frosty::Application::Get().GetWorld();

		// Writing score here since I can't get to it in other state.
		std::ofstream scoreFile;
		//Write at the end of the file
		scoreFile.open("Highscore_test.txt", std::ios::app);
		scoreFile << world->GetComponent<Frosty::ECS::CPlayer>(e.GetEntity()).Score << "\n";
		scoreFile.close();

		world->PauseGame();
		m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameOverState)), true);
	}

	void GameState::OnGameWinEvent(Frosty::WinEvent& e)
	{
		auto& world = Frosty::Application::Get().GetWorld();
		// Writing score here since I can't get to it in other state.
		std::ofstream scoreFile;
		//Write at the end of the file
		scoreFile.open("Highscore_test.txt", std::ios::app);
		scoreFile << world->GetComponent<Frosty::ECS::CPlayer>(e.GetEntity()).Score << "\n";
		scoreFile.close();

		world->PauseGame();
		m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameWinState)), true);
	}
}

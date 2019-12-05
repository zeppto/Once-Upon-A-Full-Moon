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
	}

	void GameState::OnInput()
	{
		if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		{
			auto& world = Frosty::Application::Get().GetWorld();
			world->PauseGame();
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GamePauseState)));
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
			OnGameOverEvent();
			break;
		case Frosty::EventType::Win:
			OnGameWinEvent();
			break;
		}
	}

	void GameState::OnGameOverEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameOverState)), true);
		world->PauseGame();
	}

	void GameState::OnGameWinEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameWinState)), true);
		world->PauseGame();
	}
}

#include "mcspch.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "GameOverState.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"

namespace MCS
{
	GameOverState::GameOverState()
	{
		m_App = &Frosty::Application::Get();
		Frosty::EventBus::GetEventBus()->Subscribe<GameOverState, Frosty::BaseEvent>(this, &GameOverState::OnEvent);
	}

	GameOverState::~GameOverState()
	{
	}

	void GameOverState::Initiate()
	{
	}

	void GameOverState::OnInput()
	{
	}

	void GameOverState::OnUpdate()
	{
		// Game Over 
		// Game Win
	}

	void GameOverState::OnEvent(Frosty::BaseEvent& e)
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

	void GameOverState::OnGameOverEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		m_GameOverGui = m_App->Get().GetWorld()->CreateEntity();
		Frosty::UILayout UILayout(2, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Game Over", glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(590.0f, 400.0f), "Exit", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		world->AddComponent<Frosty::ECS::CGUI>(m_GameOverGui, UILayout);
	}

	void GameOverState::OnGameWinEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		m_GameOverGui = m_App->Get().GetWorld()->CreateEntity();
		Frosty::UILayout UILayout(2, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Victory", glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(590.0f, 400.0f), "Exit", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		world->AddComponent<Frosty::ECS::CGUI>(m_GameOverGui, UILayout);
	}

	void GameOverState::DeleteGameOverGui()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_GameOverGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_GameOverGui);
		}
	}
}
#include "mcspch.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "GameOverState.hpp"

namespace MCS
{
	GameOverState::GameOverState()
	{
		m_App = &Frosty::Application::Get();
		OnGameOverEvent();
	}

	GameOverState::~GameOverState()
	{
		DeleteGameOverGui();
	}

	void GameOverState::Initiate()
	{
	}

	void GameOverState::OnInput()
	{
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		//FY_INFO("{0} : {1}", x, y);

		if (x > 920.0f && x < 1000.0f && y > 550.0f && y < 610.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
			}
		}
	}

	void GameOverState::OnUpdate()
	{
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

	void GameOverState::DeleteGameOverGui()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_GameOverGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_GameOverGui);
		}
	}
}

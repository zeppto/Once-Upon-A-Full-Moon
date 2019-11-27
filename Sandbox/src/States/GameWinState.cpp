#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GameWinState.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"

namespace MCS
{
	GameWinState::GameWinState()
	{
		m_App = &Frosty::Application::Get();
		OnGameWinEvent();
	}

	GameWinState::~GameWinState()
	{
		DeleteGameWinGui();
	}

	void GameWinState::Initiate()
	{
	}

	void GameWinState::OnInput()
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

	void GameWinState::OnUpdate()
	{
	}

	void GameWinState::OnGameWinEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();

		m_GameWinGui = m_App->Get().GetWorld()->CreateEntity();
		Frosty::UILayout UILayout(2, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Game Over", glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(590.0f, 400.0f), "Exit", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		world->AddComponent<Frosty::ECS::CGUI>(m_GameWinGui, UILayout);
	}

	void GameWinState::DeleteGameWinGui()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_GameWinGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_GameWinGui);
		}
	}
}

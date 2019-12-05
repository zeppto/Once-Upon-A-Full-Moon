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
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_GameWinGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_GameWinGui);
		}
	}

	void GameWinState::Initiate()
	{
	}

	void GameWinState::OnInput()
	{
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();

		if (x > 730.0f && x < 1215.0f && y > 565.0f && y < 605.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				auto& world = Frosty::Application::Get().GetWorld();
				Frosty::EventBus::GetEventBus()->Publish<Frosty::ResetEvent>(Frosty::ResetEvent());
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(MenuState)), true);
			}
		}
		else if (x > 860.f && x < 1085.0f && y > 445.0f && y < 505.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("HighScore");
			}
		}
		else if (x > 860.f && x < 1090.0f && y > 350.0f && y < 395.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
			}
		}
	}

	void GameWinState::OnUpdate()
	{
		if (m_ButtonsLoaded)
		{
			auto& world = Frosty::Application::Get().GetWorld();
			auto& gamePauseGui = world->GetComponent<Frosty::ECS::CGUI>(m_GameWinGui);
			float x = Frosty::InputManager::GetMouseX();
			float y = Frosty::InputManager::GetMouseY();

			if (x > 730.0f && x < 1215.0f && y > 565.0f && y < 605.0f)
			{
				gamePauseGui.Layout.texts.at(1).SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > 860.f && x < 1085.0f && y > 445.0f && y < 505.0f)
			{
				gamePauseGui.Layout.texts.at(2).SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > 860.f && x < 1090.0f && y > 350.0f && y < 395.0f)
			{
				gamePauseGui.Layout.texts.at(3).SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			}
			else
			{
				gamePauseGui.Layout.texts.at(1).SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				gamePauseGui.Layout.texts.at(2).SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				gamePauseGui.Layout.texts.at(3).SetColor(glm::vec4(0.8f, 0.0f, 0.0f, 0.0f));
			}
		}
	}

	void GameWinState::OnGameWinEvent()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_GameWinGui = m_App->Get().GetWorld()->CreateEntity();

		m_UILayout = Frosty::UILayout(4, 1);

		std::string GameOver = "Victory";
		std::string Return = "Return to Main Menu";
		std::string Score = "HighScore";
		std::string Exit = "Exit Game";

		float posX1 = (960 / 1.5f) - (GameOver.size() / 2) * 18;
		float posX2 = (960 / 1.5f) - (Return.size() / 2) * 17;
		float posX3 = (960 / 1.5f) - (Score.size() / 2) * 17;
		float posX4 = (960 / 1.5f) - (Exit.size() / 2) * 17;

		m_UILayout.AddText(glm::vec2(posX1, 525.f), GameOver, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX2, 400.0f), Return, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX3, 325.0f), Score, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX4, 250.0f), Exit, glm::vec3(0.8f, 0.0f, 0.0f), 1.0f);
		m_UILayout.AddSprite(glm::vec2(640.0f, 360.0f), glm::vec2(25.6f, 14.4f), "GameWin", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		world->AddComponent<Frosty::ECS::CGUI>(m_GameWinGui, m_UILayout);
		m_ButtonsLoaded = true;
	}
}

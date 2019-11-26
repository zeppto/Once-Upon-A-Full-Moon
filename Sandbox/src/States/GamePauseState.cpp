#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"

namespace MCS
{
	GamePauseState::GamePauseState()
	{
		m_App = &Frosty::Application::Get();
	}

	GamePauseState::~GamePauseState()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_GamePauseGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_GamePauseGui);
		}
	}

	void GamePauseState::Initiate()
	{
		m_App = &Frosty::Application::Get();
		m_App->PushOverlay(FY_NEW(GamePauseLayer));

		InitiateGui();
	}

	void GamePauseState::OnInput()
	{
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		//FY_INFO("{0} : {1}", x, y);

		if (x > 880.f && x < 1060.0f && y > 560.0f && y < 610.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("GAME STAGE NEXT");
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)), true);
			}
		}
		/*else if (x > 750.f && x < 1240.0f && y > 450.0f && y < 500.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("MENU STAGE NEXT");
				auto& world = Frosty::Application::Get().GetWorld();

				world->PauseGame();
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(MenuState)), true);
			}
		}*/
		else if (x > 920.f && x < 1000.0f && y > 350.0f && y < 400.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("EXIT STAGE NEXT");
				Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
			}
		}
	}

	void GamePauseState::OnUpdate()
	{
	}

	void GamePauseState::InitiateGui()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_GamePauseGui = m_App->Get().GetWorld()->CreateEntity();

		Frosty::UILayout UILayout(3, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Game Pause", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(590.0f, 400.0f), "Resume", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		//UILayout.AddText(glm::vec2(500.0f, 325.0f), "Return to Main Menu", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(610.0f, 250.0f), "Exit", glm::vec3(1.0f, 0.0f, 0.0f), 1.0f);
		//UILayout.AddSprite(glm::vec2(550.0f, 525.f), glm::vec2(1, 1), "", glm::vec4(1.0f));
		world->AddComponent<Frosty::ECS::CGUI>(m_GamePauseGui, UILayout);
	}
}

#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"

namespace MCS
{
	GamePauseState::GamePauseState()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		m_App = &Frosty::Application::Get();

		/*if (m_App->GameIsRunning())
		{
			m_App->StopGame(true);
		}*/
	}

	GamePauseState::~GamePauseState()
	{
		if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_TempGamePauseGui))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(m_TempGamePauseGui);
		}
		if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_GamePauseGui))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(m_GamePauseGui);
		}
	}

	void GamePauseState::Initiate()
	{
		InitiateGui();
	}

	void GamePauseState::OnInput()
	{
		//if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		//{
		//	FY_INFO("GAME STAGE NEXT");
		//	//m_App->GetStateMachine().AddState(Frosty::StateRef(new GameState()));
		//	//m_App->GetStateMachine().RemoveState();
		//	/*if (!m_App->GameIsRunning())
		//	{
		//		m_App->StartGame(true);
		//	}*/
		//}

		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		//FY_INFO("{0} : {1}", x, y);

		if (x > 880.f && x < 1060.0f && y > 560.0f && y < 610.0f)
		{
			FY_INFO("GREEN 1");
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("GAME STAGE NEXT");
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)));
			}
		}
		else if (x > 750.f && x < 1240.0f && y > 450.0f && y < 500.0f)
		{
			FY_INFO("YELLOW 2");
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("MENU STAGE NEXT");
				// Destroy Scene ...

				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(MenuState)));
			}
		}
		else if (x > 920.f && x < 1000.0f && y > 350.0f && y < 400.0f)
		{
			FY_INFO("RED 3");
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
		m_GamePauseGui = m_App->Get().GetWorld()->CreateEntity();

		Frosty::UILayout UILayout(4, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Game Pause");
		UILayout.AddText(glm::vec2(590.0f, 400.0f), "Resume");
		UILayout.AddText(glm::vec2(500.0f, 325.0f), "Return to Main Menu");
		UILayout.AddText(glm::vec2(610.0f, 250.0f), "Exit");
		m_World->AddComponent<Frosty::ECS::CGUI>(m_GamePauseGui, UILayout);

		// Temp
		m_TempGamePauseGui = m_World->CreateEntity();

		Frosty::UILayout UILayout2(1, 0);
		UILayout2.AddText(glm::vec2(25.0f, 220.0f), "GamePauseState", glm::vec3(1.0f, 0.0f, 0.0f), 1.5f);
		m_World->AddComponent<Frosty::ECS::CGUI>(m_TempGamePauseGui, UILayout2);
	}
}

#include "mcspch.hpp"
#include "States/MenuState.hpp"
#include "States/GameState.hpp"
#include "Frosty/Core/Application.hpp"

namespace MCS
{
	MenuState::MenuState()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		InitiateButtons();
	}

	MenuState::~MenuState()
	{
		if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_TempMenuGui))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(m_TempMenuGui);
		}
		if (!m_World->HasComponent<Frosty::ECS::CDestroy>(m_MenuGui))
		{
			m_World->AddComponent<Frosty::ECS::CDestroy>(m_MenuGui);
		}
	}

	void MenuState::Initiate()
	{
		m_App = &Frosty::Application::Get();
		m_App->PushOverlay(FY_NEW(MenuLayer));
	}

	void MenuState::OnInput()
	{
		/*if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT) && Frosty::InputManager::IsKeyPressed(FY_KEY_F))
		{
			FY_INFO("GAME STAGE NEXT");
			m_App->GetStateMachine().AddState(Frosty::StateRef(new GameState()));
		}*/
		/*else if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT) && Frosty::InputManager::IsKeyPressed(FY_KEY_G))
		{
			FY_INFO("SETTING STAGE NEXT");
		}*/
		/*else if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT) && Frosty::InputManager::IsKeyPressed(FY_KEY_H))
		{
			Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
		}*/

		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		//FY_INFO("{0} : {1}", x, y);

		// TODO
		// FIx HardCoded "Button" Positions ...
		if (x > 920.f && x < 990.0f && y > 540.0f && y < 600.0f)
		{
			FY_INFO("GREEN 1");
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("GAME STAGE NEXT");
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)));
			}
		}
		else if (x > 825.f && x < 1025.0f && y > 450.0f && y < 500.0f)
		{
			FY_INFO("YELLOW 2");
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("SETTINGS STAGE NEXT");
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

	void MenuState::OnUpdate()
	{
	}

	void MenuState::InitiateButtons()
	{
		m_MenuGui = m_App->Get().GetWorld()->CreateEntity();

		Frosty::UILayout UILayout(4, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Main Menu");
		UILayout.AddText(glm::vec2(610.0f, 400.0f), "Play");
		UILayout.AddText(glm::vec2(575.0f, 325.0f), "Settings");
		UILayout.AddText(glm::vec2(610.0f, 250.0f), "Exit");
		m_World->AddComponent<Frosty::ECS::CGUI>(m_MenuGui, UILayout);

		// TEMP
		m_TempMenuGui = m_World->CreateEntity();
		Frosty::UILayout UILayout2(1, 0);
		UILayout2.AddText(glm::vec2(25.0f, 220.0f), "MenuState", glm::vec3(1.0f, 1.0f, 0.0f), 1.5f);
		m_World->AddComponent<Frosty::ECS::CGUI>(m_TempMenuGui, UILayout2);
	}
}

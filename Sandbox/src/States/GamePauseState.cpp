#include "mcspch.hpp"
#include "MenuState.hpp"
#include "GameState.hpp"
#include "GamePauseState.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"

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

		if (!m_ButtonsLoaded)
		{
			InitiateGui();
			m_ButtonsLoaded = true;
		}
	}

	void GamePauseState::OnInput()
	{
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();

		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();

		if (Frosty::InputManager::IsKeyPressed(FY_KEY_ESCAPE))
		{
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)), true);
		}
		else if (x > (580.0f * width) && x < (700.0f * width) && y >(410.0f * height) && y < (440.0f * height))
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)), true);
			}
		}
		else if (x > (490.0f * width) && x < (810.0f * width) && y >(335 * height) && y < (365 * height))
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				auto& world = Frosty::Application::Get().GetWorld();
				Frosty::EventBus::GetEventBus()->Publish<Frosty::ResetEvent>(Frosty::ResetEvent());
				m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(MenuState)), true);
			}
		}
		else if (x > (610.0f * width) && x < (665.0f * width) && y >(260.0f * height) && y < (290 * height))
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
			}
		}
	}

	void GamePauseState::OnUpdate()
	{
		if (m_ButtonsLoaded)
		{
			auto& world = Frosty::Application::Get().GetWorld();
			auto& gamePauseGui = world->GetComponent<Frosty::ECS::CGUI>(m_GamePauseGui);
			float x = Frosty::InputManager::GetMouseX();
			float y = Frosty::InputManager::GetMouseY();
			float width = m_App->GetWindow().GetWidthMultiplier();
			float height = m_App->GetWindow().GetHeightMultiplier();

			if (x > (580.0f * width) && x < (700.0f * width) && y >(410.0f * height) && y < (440.0f * height))
			{
				gamePauseGui.Layout.texts[1].SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > (490.0f * width) && x < (810.0f * width) && y >(335 * height) && y < (365 * height))
			{
				gamePauseGui.Layout.texts[2].SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			}
			else if (x > (610.0f * width) && x < (665.0f * width) && y >(260.0f * height) && y < (290 * height))
			{
				gamePauseGui.Layout.texts[3].SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			}
			else
			{
				gamePauseGui.Layout.texts[1].SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				gamePauseGui.Layout.texts[2].SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
				gamePauseGui.Layout.texts[3].SetColor(glm::vec4(0.8f, 0.0f, 0.0f, 0.0f));
			}
		}
	}

	void GamePauseState::InitiateGui()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		m_GamePauseGui = m_App->Get().GetWorld()->CreateEntity();

		m_UILayout = Frosty::UILayout(4, 1);

		std::string GamePause = "Game Pause";
		std::string Resume    = "Resume";
		std::string Return    = "Return to Main Menu";
		std::string Exit      = "Exit";

		float width = m_App->GetWindow().GetWidthMultiplier();
		float height = m_App->GetWindow().GetHeightMultiplier();
		float middleX = float(m_App->GetWindow().GetWidth() / 2);
		float middleY = float(m_App->GetWindow().GetHeight() / 2);

		float posX1 = (middleX / width) - (GamePause.size() / 2) * 18;
		float posX2 = (middleX / width) - (Resume.size()    / 2) * 19;
		float posX3 = (middleX / width) - (Return.size()    / 2) * 17;
		float posX4 = (middleX / width) - (Exit.size()      / 2) * 17;

		m_UILayout.AddText(glm::vec2(posX1, middleY / height + 175.0f), GamePause, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX2, middleY / height + 50.0f), Resume, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX3, middleY / height - 25.0f), Return, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		m_UILayout.AddText(glm::vec2(posX4, middleY / height - 100.0f), Exit, glm::vec3(0.8f, 0.0f, 0.0f), 1.0f);
		m_UILayout.AddSprite(glm::vec2(640.0f, 360.0f), glm::vec2(25.6f, 14.4f), "red", glm::vec4(0.2f, 0.2f, 0.2f, 0.4f));

		world->AddComponent<Frosty::ECS::CGUI>(m_GamePauseGui, m_UILayout);
	}
}

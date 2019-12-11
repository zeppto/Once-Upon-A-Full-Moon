#include "mcspch.hpp"
#include "MenuState.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"
#include "HighScoreState.h"
#include "GameOverState.hpp"
#include <fstream>

MCS::HighscoreState::HighscoreState()
{
	m_App = &Frosty::Application::Get();
}

MCS::HighscoreState::~HighscoreState()
{
	auto& world = Frosty::Application::Get().GetWorld();
	if (!world->HasComponent<Frosty::ECS::CDestroy>(m_HighscoreGUI))
	{
		world->AddComponent<Frosty::ECS::CDestroy>(m_HighscoreGUI);
	}
}

void MCS::HighscoreState::Initiate()
{
	m_App = &Frosty::Application::Get();
	InitiateGUI();
}

void MCS::HighscoreState::OnInput()
{
	float x = Frosty::InputManager::GetMouseX();
	float y = Frosty::InputManager::GetMouseY();

	if (x > 600.0f && x < 800.0f && y > 30.0f && y < 80.0f)
	{
		if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
		{
			m_App->GetStateMachine().RemoveState();
		}
	}
}

void MCS::HighscoreState::OnUpdate()
{
	if (m_HighscoreGUI != nullptr)
	{
		auto& world = Frosty::Application::Get().GetWorld();
		auto& menuGui = world->GetComponent<Frosty::ECS::CGUI>(m_HighscoreGUI);
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();

		if (x > 600.0f && x < 800.0f && y > 30.0f && y < 80.0f)
		{
			menuGui.Layout.texts.at(2).SetColor(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}
		else
		{
			menuGui.Layout.texts.at(2).SetColor(glm::vec4(1.0f, 1.0f, 0.0f, 0.0f));
		}
	}
}

void MCS::HighscoreState::InitiateGUI()
{
	auto& world = Frosty::Application::Get().GetWorld();
	m_HighscoreGUI = m_App->Get().GetWorld()->CreateEntity();

	//10 for scores two for player score and "Highscore List" text. One for "Back"
	Frosty::UILayout UILayout(13, 1);

	float height = 650.0f;

	UILayout.AddText(glm::vec2(520.0f, height), "HIGHSCORE LIST", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);

	//Allocate 11 spots for if list is full and the score doesn't make it in we still need to sort.
	std::vector<int> scores(11);

	//Read highscore file
	std::string line;
	std::ifstream iScores("Highscore_test.txt");
	height -= 70.0f;

	int count = 0;
	if (iScores.is_open())
	{
		while (std::getline(iScores, line))
		{
			if (count < scores.size())
			{
				scores[count] = (std::stoi(line));
			}
			else
			{
				scores.push_back((std::stoi(line)));
			}
			count++;
		}
		iScores.close();
	}
	int scoreLoc = count - 1;
	//Latest added at this point is player's score
	UILayout.AddText(glm::vec2(540.0f, height- 550.0f), "Your score: " + std::to_string(scores[scoreLoc]), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
	UILayout.AddText(glm::vec2(400.0f, height - 550.0f), "Back", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
	UILayout.AddSprite(glm::vec2(640.0f, 360.0f), glm::vec2(25.6f, 14.4f), "Background", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	//Sort from high to low
	std::sort(scores.begin(), scores.end(), std::greater<int>());

	//Only keep the ten highest scores.
	int listLen = 0;
	if (count < 10)
	{
		listLen = count;
	}
	else
	{
		listLen = 10;
	}

	std::ofstream oScores;
	//Open with truncate in order to delete old content.
	oScores.open("Highscore_test.txt", std::ofstream::out | std::ofstream::trunc);
	for (int i = 0; i < listLen; i++)
	{
		//Write it to the text file.
		oScores << std::to_string(scores[i]) + "\n";
	}
	oScores.close();

	//Output the final highscore list
	for (int i = 0; i < listLen; i++)
	{
		UILayout.AddText(glm::vec2(600.0f, height), std::to_string(i+1) + "   " + std::to_string(scores[i]), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		height -= 40.0f;
	}

	world->AddComponent<Frosty::ECS::CGUI>(m_HighscoreGUI, UILayout);
	scores.clear();
}

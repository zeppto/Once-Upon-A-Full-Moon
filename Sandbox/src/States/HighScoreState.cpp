#include "mcspch.hpp"
#include "MenuState.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include "Frosty/Events/EventSystem.hpp"
#include "HighScoreState.h"
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
}

void MCS::HighscoreState::OnUpdate()
{
}

void MCS::HighscoreState::InitiateGUI()
{
	auto& world = Frosty::Application::Get().GetWorld();
	m_HighscoreGUI = m_App->Get().GetWorld()->CreateEntity();

	//10 for scores two for player score and "Highscore List" text.
	Frosty::UILayout UILayout(12, 0);

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
			scores[count] = (std::stoi(line));
			count++;
		}
		iScores.close();
	}
	//Latest added at this point is player's score
	UILayout.AddText(glm::vec2(540.0f, height-550), "Your score: " + std::to_string(scores[count-1]), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);

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

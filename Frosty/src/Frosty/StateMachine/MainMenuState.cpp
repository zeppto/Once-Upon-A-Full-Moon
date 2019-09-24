#include "fypch.hpp"
#include "MainMenuState.h"

Frosty::MainMenuState::MainMenuState(Application * app)
{
	this->app = app;
	//auto stuffsies = &Frosty::Application::Get();

	Initiate();
}

void Frosty::MainMenuState::Initiate()
{
	//TODO:
	//Load MainMenu Texture.
	std::cout << "MainMenu texture Loaded" << std::endl;
	//TODO set up buttons.
	std::cout << "Buttons loaded" << std::endl;

}

void Frosty::MainMenuState::OnInput()
{
	//TODO:

	//If Options is clicked.
	if (app->GetInputManager().IsKeyPressed(GLFW_KEY_O))
	{
		std::cout << "Options was clicked!!" << std::endl;
	}

	//If Start is clicked.

	//If Scoreboard is clicked.

}

void Frosty::MainMenuState::OnUpdate()
{
	if (app->GetInputManager().IsKeyPressed(GLFW_KEY_O))
	{
		std::cout << "Options was clicked!!" << std::endl;
	}
	//TODO: Check if button clicked? Could just be done with input
}

#include <mcspch.hpp>

#include "MainMenuState.h"



namespace MCS
{

	MainMenuState::MainMenuState(Frosty::Application* app)
	{
		this->app = app;
		/*auto stuffsies = &Frosty::Application::Get();*/

		Initiate();
	}

	MainMenuState::~MainMenuState()
	{
	}

	void MainMenuState::Initiate()
	{
		//TODO:
		//Load MainMenu Texture.
		std::cout << "MainMenu texture Loaded" << std::endl;
		//TODO set up buttons.
		std::cout << "Buttons loaded" << std::endl;

	}

	void MainMenuState::OnInput()
	{
		//TODO:

		//If Options is clicked.
		//if (app->get.IsKeyPressed(GLFW_KEY_O))
		//{
		//	std::cout << "Options was clicked!!" << std::endl;
		//}

		//If Start is clicked.

		//If Scoreboard is clicked.

	}

	void MainMenuState::OnUpdate()
	{
		////TODO: Replace keys with button inputs later.
		//if (app->GetInputManager().IsKeyPressed(GLFW_KEY_O))
		//{
		//	//TODO: Actual state for options menu here.
		//	std::cout << "Options was clicked!!" << std::endl;

		//	app->getStateMachine().AddState(Frosty::StateRef(FY_NEW OptionMenuState()));
		//}

		//if (app->GetInputManager().IsKeyPressed(GLFW_KEY_S))
		//{
		//	//TODO: Actual state for Game Start here.
		//	std::cout << "Start was clicked!!" << std::endl;
		//	app->getStateMachine().AddState(Frosty::StateRef(FY_NEW PlayingState()));
		//}
	}
}

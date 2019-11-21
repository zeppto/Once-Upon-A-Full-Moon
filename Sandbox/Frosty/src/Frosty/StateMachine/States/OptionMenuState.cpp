#include "fypch.hpp"
#include "OptionMenuState.h"

Frosty::OptionMenuState::OptionMenuState()
{
	app = &Frosty::Application::Get();
	Initiate();
}

Frosty::OptionMenuState::~OptionMenuState()
{
}

void Frosty::OptionMenuState::Initiate()
{
	//~~ From 0 to 100
	this->volume = 50;
	this->brightness = 50;
	//~~ Bool
	this->fullscreen = false;
}

void Frosty::OptionMenuState::OnInput()
{
	app->setMainMenuReturn(true);
}

void Frosty::OptionMenuState::OnUpdate()
{
}

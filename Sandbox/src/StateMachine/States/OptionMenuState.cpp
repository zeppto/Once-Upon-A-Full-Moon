#include <mcspch.hpp>
#include "OptionMenuState.h"

namespace MCS
{

	OptionMenuState::OptionMenuState()
	{
		app = &Frosty::Application::Get();
		Initiate();
	}

	OptionMenuState::~OptionMenuState()
	{
	}

	void OptionMenuState::Initiate()
	{
		//~~ From 0 to 100
		this->volume = 50;
		this->brightness = 50;
		//~~ Bool
		this->fullscreen = false;
	}

	void OptionMenuState::OnInput()
	{
		//app->setMainMenuReturn(true);
	}

	void OptionMenuState::OnUpdate()
	{
	}

}
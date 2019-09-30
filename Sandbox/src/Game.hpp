#ifndef FROSTY_HPP
#define FROSTY_HPP

#include <Frosty.h>
#include <iostream>
#include "imgui/imgui.h"
#include "ObjectHandler/Player/Player.hpp"



class ExampleLayerA : public Frosty::Layer
{

private:
	Player m_Player;
	Player m_Player2;

public:
	ExampleLayerA()
		: Layer("Example")
	{

	}
	void OnUpdate() override
	{



		m_Player.UpdatePlayer();




	//	FY_INFO("ExampleLayer::Update");
	}
	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	bool OnEvent(Frosty::BaseEvent& event) override
	{
		FY_TRACE("ExampleLayerA event handled!");
		//FY_TRACE("{0}", event);
		return true;
	}
};

class Game : public Frosty::Application
{
public:
	Game();
	virtual ~Game();

};

#endif // !FROSTY_HPP

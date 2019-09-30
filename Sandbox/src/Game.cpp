#include <mcspch.hpp>
#include "Game.hpp"

namespace MCS
{
	Game::Game()
	{
		PushLayer(FY_NEW InspectorLayer());


		m_App = &Frosty::Application::Get();
		auto& scene = m_App->GetScene();
		auto& obj = scene->CreateEntity();
		scene->AddComponent<Frosty::ECS::CTransform>(obj);
		scene->GetComponent<Frosty::ECS::CTransform>(obj);
		scene->RemoveComponent<Frosty::ECS::CTransform>(obj);
		scene->RemoveEntity(obj);
	}

	Game::~Game()
	{

	}
}

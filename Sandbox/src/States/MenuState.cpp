#include "mcspch.hpp"
#include "States/MenuState.hpp"
#include "States/GameState.hpp"
#include "Frosty/Core/Application.hpp"

#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"

#include "Systems/FollowSystem.hpp"
#include "Systems/AttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/LootingSystem.hpp"
#include "Systems/LevelSystem.hpp"
#include "Systems/HealthBarSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"
#include "Systems/GUISystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/AISystem.hpp"
#include "Systems/WitchCircleSystem.hpp"

namespace MCS
{
	MenuState::MenuState()
	{
	}

	MenuState::~MenuState()
	{
		auto& world = Frosty::Application::Get().GetWorld();
		if (!world->HasComponent<Frosty::ECS::CDestroy>(m_MenuGui))
		{
			world->AddComponent<Frosty::ECS::CDestroy>(m_MenuGui);
		}
	}

	void MenuState::Initiate()
	{
		m_App = &Frosty::Application::Get();
		m_App->PushOverlay(FY_NEW(MenuLayer));

		// -------------------------------------------------------------

		if (!m_App->MenuLoaded())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			// Add systems
			world->AddSystem<LevelSystem>();
			world->AddSystem<CameraSystem>();
			world->AddSystem<LightSystem>();
			world->AddSystem<AnimationSystem>();
			world->AddSystem<RenderSystem>();
			world->AddSystem<PlayerControllerSystem>();
			world->AddSystem<PhysicsSystem>();
			world->AddSystem<FollowSystem>();
			world->AddSystem<AttackSystem>();
			world->AddSystem<CombatSystem>();
			world->AddSystem<DestroySystem>();
			world->AddSystem<HealthBarSystem>();
			world->AddSystem<AISystem>();

			Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
			NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);
			retSystem = world->AddSystem<ParticleSystem>();
			ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);

			world->AddSystem<BossBehaviorSystem>();
			world->AddSystem<GUISystem>();
			world->AddSystem<LootingSystem>();
			world->AddSystem<WitchCircleSystem>();

			world->Awake();
			particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));

			m_App->SetMenuLoad(true);
		}
		InitiateButtons();
	}

	void MenuState::OnInput()
	{
		float x = Frosty::InputManager::GetMouseX();
		float y = Frosty::InputManager::GetMouseY();
		FY_INFO("{0} : {1}", x, y);
		
		if (x > 915.0f && x < 1005.0f && y > 540.0f && y < 600.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				auto& world = Frosty::Application::Get().GetWorld();
				world->PlayGame();
				FY_INFO("GAME STAGE NEXT");
				//m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(GameState)));
			}
		}
		else if (x > 830.f && x < 1030.0f && y > 450.0f && y < 500.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("SETTINGS STAGE NEXT");
			}
		}
		else if (x > 920.f && x < 1000.0f && y > 350.0f && y < 400.0f)
		{
			if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
			{
				FY_INFO("EXIT STAGE NEXT");
				//Frosty::EventBus::GetEventBus()->Publish<Frosty::WindowCloseEvent>(Frosty::WindowCloseEvent());
			}
		}
	}

	void MenuState::OnUpdate()
	{
	}

	void MenuState::InitiateButtons()
	{
		m_App = &Frosty::Application::Get();
		auto& world = Frosty::Application::Get().GetWorld();
		m_MenuGui = m_App->Get().GetWorld()->CreateEntity();

		Frosty::UILayout UILayout(4, 0);
		UILayout.AddText(glm::vec2(550.0f, 525.f), "Main Menu", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(610.0f, 400.0f), "Play", glm::vec3(1.0f, 1.0f, 0.0f), 1.0f);
		UILayout.AddText(glm::vec2(575.0f, 325.0f), "Settings", glm::vec3(0.4f, 0.4f, 0.4f), 1.0f);
		UILayout.AddText(glm::vec2(610.0f, 250.0f), "Exit", glm::vec3(0.8f, 0.0f, 0.0f), 1.0f);
		world->AddComponent<Frosty::ECS::CGUI>(m_MenuGui, UILayout);

		//auto& camEntity = world->GetSceneCamera();
		//world->GetComponent<Frosty::ECS::CCamera>(camEntity).
	}
}

#include "mcspch.hpp"
#include "imgui/imgui.h"
#include "States/LoadingState.hpp"
#include "States/MenuState.hpp"
#include "Frosty/Core/Application.hpp"

#include "Systems/RenderSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/LightSystem.hpp"
#include "Systems/GUISystem.hpp"

#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"

#include "Systems/AttackSystem.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/DestroySystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/HealthBarSystem.hpp"
#include "Systems/NavigationSystem.hpp"
#include "Systems/ParticleSystem.hpp"
#include "Systems/BossBehaviorSystem.hpp"
#include "Systems/LootingSystem.hpp"
#include "Systems/AISystem.hpp"

#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

namespace MCS
{
	LoadingState::LoadingState()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
	}

	void LoadingState::Initiate()
	{
		//FY_INFO("Initializing");
		//auto& world = Frosty::Application::Get().GetWorld();
		//InitiateSystems();
		//world->Awake();
		//
		//Frosty::ECS::BaseSystem* retSystem;
		//retSystem = world->AddSystem<ParticleSystem>();
		//ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);
		//particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));
		//
		//FY_INFO("Initialization completed");
		//m_IsInitialized = true;

		//InitiateSystems();
		//m_IsInitialized = true;
	}

	void LoadingState::OnInput()
	{
	}

	void LoadingState::OnUpdate()
	{
		if (m_IsInitialized)
		{
			m_App->GetStateMachine().AddState(Frosty::StateRef(FY_NEW(MenuState)));
		}
		else
		{
			// SetLoadingScreen
		}
	}

	void LoadingState::InitiateSystems()
	{
		//auto& world = Frosty::Application::Get().GetWorld();
		//world->AddSystem<RenderSystem>();
		//world->AddSystem<CameraSystem>();
		//world->AddSystem<LightSystem>();
		//world->AddSystem<GUISystem>();
		
		//world->AddSystem<PhysicsSystem>();
		//world->AddSystem<PlayerControllerSystem>();
		
		//world->AddSystem<AttackSystem>();
		//world->AddSystem<CombatSystem>();
		//world->AddSystem<DestroySystem>();
		//world->AddSystem<AnimationSystem>();
		//world->AddSystem<HealthBarSystem>();
		
		//Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		//NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);

		//retSystem = world->AddSystem<ParticleSystem>();
		//ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);
		//particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));

		//world->AddSystem<BossBehaviorSystem>();
		//world->AddSystem<LootingSystem>();
		//world->AddSystem<AISystem>();

		// -------------------------------

		//auto& world = Frosty::Application::Get().GetWorld();
		//// Add systems
		//world->AddSystem<CameraSystem>();
		//world->AddSystem<LightSystem>();
		//world->AddSystem<AnimationSystem>();
		//world->AddSystem<RenderSystem>();
		//world->AddSystem<PlayerControllerSystem>();
		//world->AddSystem<PhysicsSystem>();
		//world->AddSystem<AttackSystem>();
		//world->AddSystem<CombatSystem>();
		//world->AddSystem<DestroySystem>();
		//world->AddSystem<HealthBarSystem>();
		//world->AddSystem<AISystem>();
		//Frosty::ECS::BaseSystem* retSystem = world->AddSystem<NavigationSystem>();
		//NavigationSystem* navSystem = dynamic_cast<NavigationSystem*>(retSystem);

		//retSystem = world->AddSystem<ParticleSystem>();
		//ParticleSystem* particleSystem = dynamic_cast<ParticleSystem*>(retSystem);

		//world->AddSystem<BossBehaviorSystem>();
		//world->AddSystem<GUISystem>();
		//world->AddSystem<LootingSystem>();

		//world->Awake();
		//particleSystem->AttachGameCamera(&world->GetComponent<Frosty::ECS::CTransform>(world->GetSceneCamera()));		
	}

	void LoadingState::InitiateLoadingScreen()
	{
	}

	void LoadingState::InitiateMedia()
	{
	}
}

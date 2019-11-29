#include <mcspch.hpp>
#include "Game.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/CombatEvent.hpp"

#include "Systems/LightSystem.hpp"
#include "Systems/CameraSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/PhysicsSystem.hpp"
#include "Systems/PlayerControllerSystem.hpp"
//#include "LevelHandeler/MapGenerator.hpp"

//temp
//#include "LevelHandeler/LevelsHardCoded.hpp"
//#include "Systems/FollowSystem.hpp"
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

//#include "LevelHandeler/LevelFileFormat.hpp"

//#include "States/LoadingState.hpp"
#include "States/MenuState.hpp"

namespace MCS
{
	Game::Game()
	{
		auto& state = Application::Get().GetStateMachine();
		//state.AddState(Frosty::StateRef(FY_NEW(LoadingState)));
		state.AddState(Frosty::StateRef(FY_NEW(MenuState)));
		//Application::Get().StartGame(true);

#ifdef FY_DEBUG
		PushLayer(FY_NEW InspectorLayer());
#else
		Application::Get().StartGame(true);
#endif
	}

	Game::~Game()
	{
	}
}

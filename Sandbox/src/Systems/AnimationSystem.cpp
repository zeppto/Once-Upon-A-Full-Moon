#include <mcspch.hpp>
#include "AnimationSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"


void MCS::AnimationSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAnimController>(), true);
}

void MCS::AnimationSystem::OnUpdate()
{
}

void MCS::AnimationSystem::OnEvent(Frosty::BaseEvent& e)
{
	switch (e.GetEventType())
	{
	case Frosty::EventType::BasicAttack:
		OnBasicAttackEvent(static_cast<Frosty::BasicAttackEvent&>(e));
		break;
	case Frosty::EventType::Dash:
		break;
	}
}

void MCS::AnimationSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
}

void MCS::AnimationSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
}

void MCS::AnimationSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
}

std::string MCS::AnimationSystem::GetInfo() const
{
	return std::string();
}

void MCS::AnimationSystem::OnBasicAttackEvent(Frosty::BasicAttackEvent& e)
{
	auto& it = p_EntityMap.find(e.GetEntity());

	auto& world = Frosty::Application::Get().GetWorld();
	if (&world->GetComponent<Frosty::ECS::CEnemy>(it->first))
	{
		////TODO: Determine if wolf or cultist somehow!
		auto map = *Frosty::AssetManager::GetAnimationMap();
		world->GetComponent<Frosty::ECS::CAnimController>(it->first).currAnim = &map.at("Wolf_atk");
	}
	else
	{
		//If not enemy it's player.
		auto map = *Frosty::AssetManager::GetAnimationMap();
		Frosty::ECS::CAnimController controller = world->GetComponent<Frosty::ECS::CAnimController>(it->first);
		controller.currAnim = &map.at("p_atk");
		
	}
}

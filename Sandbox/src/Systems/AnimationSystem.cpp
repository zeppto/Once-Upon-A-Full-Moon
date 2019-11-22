#include <mcspch.hpp>
#include "AnimationSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"


void MCS::AnimationSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAnimController>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CDash>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
}

void MCS::AnimationSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		//AKA if player...For Now
		//May become a problem if enemies get dash.
		if (m_Dash[i])
		{
			if (!m_Dash[i]->Active)
			{
				if (m_Physics[i]->Direction.x != 0.0f || m_Physics[i]->Direction.y != 0.0f || m_Physics[i]->Direction.z != 0.0f)
				{
					m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("NewRun");
				}
				else
				{

				}
			}
		}
	}
}

void MCS::AnimationSystem::OnEvent(Frosty::BaseEvent& e)
{
	switch (e.GetEventType())
	{
	case Frosty::EventType::BasicAttack:
		OnBasicAttackEvent(static_cast<Frosty::BasicAttackEvent&>(e));
		break;
	case Frosty::EventType::Dash:
		OnDashEvent(static_cast<Frosty::DashEvent&>(e));
		break;
	}
}

void MCS::AnimationSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);
		m_AControllers[p_Total] = &world->GetComponent<Frosty::ECS::CAnimController>(entity);
		m_Dash[p_Total] = &world->GetComponent<Frosty::ECS::CDash>(entity);

		p_Total++;
	}
}

void MCS::AnimationSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	auto& it = p_EntityMap.find(entity);

	if (it != p_EntityMap.end())
	{
		p_Total--;
		auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
		m_Transform[p_Total] = nullptr;
		m_Dash[p_Total] = nullptr;
		m_Physics[p_Total] = nullptr;

		if (p_Total > it->second)
		{
			p_EntityMap[entityToUpdate] = it->second;
		}

		p_EntityMap.erase(entity);
	}
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
	/*	auto map = *Frosty::AssetManager::GetAnimationMap();
		world->GetComponent<Frosty::ECS::CAnimController>(it->first).currAnim = &map.at("Wolf_atk");*/
	}
	else
	{
		//If not enemy it's player.
		Frosty::ECS::CAnimController * controller = &world->GetComponent<Frosty::ECS::CAnimController>(it->first);
		controller->currAnim = Frosty::AssetManager::GetAnimation("p_atk");
	}
}

void MCS::AnimationSystem::OnDashEvent(Frosty::DashEvent& e)
{
	/*auto& it = p_EntityMap.find(e.GetEntity());*/
	auto& world = Frosty::Application::Get().GetWorld();

	Frosty::ECS::CAnimController * controller = &world->GetComponent<Frosty::ECS::CAnimController>(e.GetEntity());

	controller->currAnim = Frosty::AssetManager::GetAnimation("p_atk");
}

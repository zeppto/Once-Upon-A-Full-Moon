#include <mcspch.hpp>
#include "AnimationSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include <glm/gtx/matrix_decompose.hpp >


void MCS::AnimationSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAnimController>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CDash>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPlayer>(), true);

	m_World = Frosty::Application::Get().GetWorld().get();
}

void MCS::AnimationSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		if (!m_AControllers[i]->isBusy)
		{
			if (m_Player[i])
			{
				if (!m_Dash[i]->Active)
				{
					if (m_Physics[i]->Direction.x != 0.0f || m_Physics[i]->Direction.y != 0.0f || m_Physics[i]->Direction.z != 0.0f)
					{
						if (m_AControllers[i]->currAnim->GetName() != "Scarlet_Run")
						{
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Run");
							m_AControllers[i]->animSpeed = 0.7;
							UpdateAnimOffset(m_AControllers[i]);
						}
					}
					else
					{
						if (m_AControllers[i]->currAnim->GetName() != "Scarlet_Idle")
						{
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
							m_AControllers[i]->animSpeed = 1.0;
							UpdateAnimOffset(m_AControllers[i]);
						}
					}
				}
			}
		}
		else
		{
			if (m_AControllers[i]->currAnim->GetIsFinished())
			{
				m_AControllers[i]->isBusy = false;
			}
		}
	}
}

void MCS::AnimationSystem::OnEvent(Frosty::BaseEvent& e)
{
	switch (e.GetEventType())
	{
	case Frosty::EventType::PlayAnim:
		OnPlayAnimEvent(static_cast<Frosty::PlayAnimEvent&>(e));
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
		m_Player[p_Total] = &world->GetComponent<Frosty::ECS::CPlayer>(entity);

		//if (m_AControllers[p_Total]->currAnim->getHoldingJoint() != nullptr)
		//{
		//	auto& weapPos = m_World->GetComponent<Frosty::ECS::CTransform>(m_Player[p_Total]->Weapon->EntityPtr);
		//	auto& playerPos = m_World->GetComponent<Frosty::ECS::CTransform>(m_Player[p_Total]->EntityPtr);

		//	//NOTE we DO want to change the value of the parent matrix pointer and NOT replace it with another pointer.
		//	//If we don't Renderer won't be able to catch updates as AddToRenderer is only run once.
		//	//Parent it.
		//	m_World->GetComponent<Frosty::ECS::CMesh>(m_Player[p_Total]->Weapon->EntityPtr).parentMatrix = m_Transform[p_Total]->GetModelMatrix();
		//	//Save the joint translation
		//	m_World->GetComponent<Frosty::ECS::CMesh>(m_Player[p_Total]->Weapon->EntityPtr).animOffset = m_AControllers[p_Total]->currAnim->getHoldingJoint();

		//}

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
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Attack1");
		UpdateAnimOffset(controller);
	}
}

void MCS::AnimationSystem::OnDashEvent(Frosty::DashEvent& e)
{
	/*auto& it = p_EntityMap.find(e.GetEntity());*/
	auto& world = Frosty::Application::Get().GetWorld();

	Frosty::ECS::CAnimController * controller = &world->GetComponent<Frosty::ECS::CAnimController>(e.GetEntity());
	if (!controller->isBusy)
	{
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Dash");
		controller->dt = 0;
		/*glm::translate(*controller->currAnim->getHoldingJoint(), glm::vec3(-1.0f, 0.0f, 0.0f));*/
		UpdateAnimOffset(controller);
	}
}

void MCS::AnimationSystem::OnPlayAnimEvent(Frosty::PlayAnimEvent& e)
{
	Frosty::ECS::CAnimController* controller = &m_World->GetComponent<Frosty::ECS::CAnimController>(e.GetEntity());

  	const unsigned int* id = e.getAnimID();

	switch (*id)
	{
	case 0:
		controller->dt = 0;
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Death");
		controller->currAnim->SetIsRepeating(false);
		controller->isBusy = true;
		break;
	case 1:
		controller->dt = 0;
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Attack1");
		controller->currAnim->SetIsRepeating(false);
		controller->currAnim->SetIsFinished(false);
		controller->isBusy = true;
		controller->animSpeed = 1.0f;
		break;
	case 2:
		controller->dt = 0;
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Attack2");
		controller->currAnim->SetIsRepeating(false);
		controller->currAnim->SetIsFinished(false);
		controller->isBusy = true;
		controller->animSpeed = 1.2f;
		break;
	case 3:
		controller->dt = 0;
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Attack3");
		controller->currAnim->SetIsRepeating(false);
		controller->currAnim->SetIsFinished(false);
		controller->isBusy = true;
		controller->animSpeed = 1.0f;
		break;
	case 4:
		controller->dt = 0;
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Attack4");
		controller->currAnim->SetIsRepeating(false);
		controller->currAnim->SetIsFinished(false);
		controller->isBusy = true;
		controller->animSpeed = 1.5f;
		break;
	}
	UpdateAnimOffset(controller);
}

void MCS::AnimationSystem::UpdateAnimOffset(Frosty::ECS::CAnimController* ctrl)
{
	/*ctrl->EntityPtr*/
	auto& wEntity = m_World->GetComponent<Frosty::ECS::CPlayer>(ctrl->EntityPtr).Weapon->EntityPtr;

	m_World->GetComponent<Frosty::ECS::CMesh>(wEntity).animOffset = ctrl->currAnim->getHoldingJoint();
	Frosty::Renderer::UpdateCMesh(wEntity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(wEntity));
}

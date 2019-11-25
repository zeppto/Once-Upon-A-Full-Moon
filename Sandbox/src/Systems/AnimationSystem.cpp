#include <mcspch.hpp>
#include "AnimationSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Events/AbilityEvent.hpp"
#include <glm/gtx/matrix_decompose.hpp >


void MCS::AnimationSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CAnimController>(), true);

	m_World = Frosty::Application::Get().GetWorld().get();
}

void MCS::AnimationSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		//May want to reset DT for idle and run too.
		if (!m_AControllers[i]->isBusy)
		{
			if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_AControllers[i]->EntityPtr))
			{
				if (!m_World->GetComponent<Frosty::ECS::CDash>(m_AControllers[i]->EntityPtr).Active)
				{
					auto& physics = m_World->GetComponent<Frosty::ECS::CPhysics>(m_AControllers[i]->EntityPtr);
					if (physics.Direction.x != 0.0f || physics.Direction.y != 0.0f || physics.Direction.z != 0.0f)
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
			else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr))
			{
				auto& enemy = m_World->GetComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr);
				auto& wType = m_World->GetComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr).Weapon->Type;

				//If it has bite it is a wolf
				if (wType == Frosty::ECS::CWeapon::WeaponType::Bite)
				{
					if (enemy.CurrentState == Frosty::ECS::CEnemy::State::Chase || enemy.CurrentState == Frosty::ECS::CEnemy::State::Escape 
						|| enemy.CurrentState == Frosty::ECS::CEnemy::State::Reset)
					{
						if (m_AControllers[i]->currAnim->GetName() != "Wolf_Running")
						{
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Wolf_Running");
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim->SetIsFinished(false);
							m_AControllers[i]->animSpeed = 1.0;
						}
					}
					else
					{
						if (m_AControllers[i]->currAnim->GetName() != "Wolf_Idle")
						{
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Wolf_Idle");
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim->SetIsFinished(false);
							m_AControllers[i]->animSpeed = 1.0;
						}
					}
				}
				else
				{
					//Otherwise it is cultist.
					if (enemy.CurrentState == Frosty::ECS::CEnemy::State::Chase || enemy.CurrentState == Frosty::ECS::CEnemy::State::Escape
						|| enemy.CurrentState == Frosty::ECS::CEnemy::State::Reset)
					{
						if (m_AControllers[i]->currAnim->GetName() != "Cultist_Run")
						{
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Cultist_Run");
							m_AControllers[i]->animSpeed = 2.0;
							UpdateAnimOffset(m_AControllers[i]);
						}
					}
					else
					{
						if (m_AControllers[i]->currAnim->GetName() != "Cultist_Idle")
						{
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Cultist_Idle");
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

		m_Transform[p_Total] = &m_World->GetComponent<Frosty::ECS::CTransform>(entity);
		m_AControllers[p_Total] = &m_World->GetComponent<Frosty::ECS::CAnimController>(entity);

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
		m_AControllers[p_Total] = nullptr;

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

	Frosty::ECS::CAnimController* controller = &world->GetComponent<Frosty::ECS::CAnimController>(e.GetEntity());
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

	if (m_World->HasComponent<Frosty::ECS::CPlayer>(e.GetEntity()))
	{
		switch (*id)
		{
		case 0:
			BeginNewAnim(controller, "Scarlet_Death");
			controller->animSpeed = 1.0f;
			break;
		case 1:
			BeginNewAnim(controller, "Scarlet_Attack1");
			controller->animSpeed = 1.0f;
			break;
		case 2:
			BeginNewAnim(controller, "Scarlet_Attack2");
			controller->animSpeed = 1.2f;
			break;
		case 3:
			BeginNewAnim(controller, "Scarlet_Attack3");
			controller->animSpeed = 1.0f;
			break;
		case 4:
			//Put temp transform here.
			BeginNewAnim(controller, "Scarlet_Attack4");
			controller->animSpeed = 1.5f;
			break;
		}
		UpdateAnimOffset(controller);
	}
	else if (m_World->HasComponent<Frosty::ECS::CEnemy>(e.GetEntity()))
	{
		auto& wType = m_World->GetComponent<Frosty::ECS::CEnemy>(e.GetEntity()).Weapon->Type;

		//If it has bite it is a wolf
		if (wType == Frosty::ECS::CWeapon::WeaponType::Bite)
		{
			switch (*id)
			{
			case 0:
				BeginNewAnim(controller, "Wolf_Death");
				controller->animSpeed = 1.0f;
			case 1:
				BeginNewAnim(controller, "Wolf_Attack");
				controller->animSpeed = 1.0f;
			}
		}
		else
		{
			//Otherwise it is cultist.
			switch (*id)
			{
			case 0:
				BeginNewAnim(controller, "Wolf_Death");
				controller->animSpeed = 1.0f;
			case 1:
				BeginNewAnim(controller, "Cultist_Attack1");
				controller->animSpeed = 1.0f;
			case 2:
				BeginNewAnim(controller, "Cultist_Attack2");
				controller->animSpeed = 2.0f;
			}
			UpdateAnimOffset(controller);
		}
	}
}

// Resets the time for the anim controller and sets the animation to not repeat and resets the animation.
void MCS::AnimationSystem::BeginNewAnim(Frosty::ECS::CAnimController* controller, const std::string& animName)
{
	controller->dt = 0;
	controller->currAnim = Frosty::AssetManager::GetAnimation(animName);
	controller->currAnim->SetIsRepeating(false);
	controller->currAnim->SetIsFinished(false);
	controller->isBusy = true;
}

void MCS::AnimationSystem::UpdateAnimOffset(Frosty::ECS::CAnimController* ctrl)
{
	/*ctrl->EntityPtr*/
	if (m_World->HasComponent<Frosty::ECS::CPlayer>(ctrl->EntityPtr))
	{
		auto& wEntity = m_World->GetComponent<Frosty::ECS::CPlayer>(ctrl->EntityPtr).Weapon->EntityPtr;

		m_World->GetComponent<Frosty::ECS::CMesh>(wEntity).animOffset = ctrl->currAnim->getHoldingJoint();
		Frosty::Renderer::UpdateCMesh(wEntity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(wEntity));
	}
	else
	{
		//Is cultist
		auto& wEntity = m_World->GetComponent<Frosty::ECS::CEnemy>(ctrl->EntityPtr).Weapon->EntityPtr;

		m_World->GetComponent<Frosty::ECS::CMesh>(wEntity).animOffset = ctrl->currAnim->getHoldingJoint();
		Frosty::Renderer::UpdateCMesh(wEntity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(wEntity));
	}
}

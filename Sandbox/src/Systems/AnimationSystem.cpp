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

		if (m_World->HasComponent<Frosty::ECS::CPlayer>(m_AControllers[i]->EntityPtr))
		{
			if (m_AControllers[i]->breakable)
			{
				if (!m_World->GetComponent<Frosty::ECS::CDash>(m_AControllers[i]->EntityPtr).Active)
				{
					auto& physics = m_World->GetComponent<Frosty::ECS::CPhysics>(m_AControllers[i]->EntityPtr);
					
					if (physics.Direction.x != 0.0f || physics.Direction.z != 0.0f)
					{
						if (m_AControllers[i]->currAnim->GetName() != "Scarlet_Run")
						{
							m_AControllers[i]->dt = 0.0f;
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Run");
							m_AControllers[i]->animSpeed = physics.Speed / 24.3f; //Set to Physics speed? To increase animSpeed when speed boost
							m_AControllers[i]->isBusy = false;
							UpdateAnimOffset(m_AControllers[i]);
						}
					}
					else if (physics.Direction.y != 0.0f)
					{
						//Scarlet is stunned!
					}
					else
					{
						if (m_AControllers[i]->currAnim->GetName() != "Scarlet_Idle" && !m_AControllers[i]->isBusy)
						{
							m_AControllers[i]->dt = 0.0f;
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Idle");
							m_AControllers[i]->animSpeed = 1.0f;
							UpdateAnimOffset(m_AControllers[i]);
						}
					}
				}
			}
			
			else if (!m_AControllers[i]->currAnim->GetIsFinished())
			{
				if (m_AControllers[i]->currAnim->GetStridePercent() > 0.55f && m_AControllers[i]->currAnim->GetName() == "Scarlet_Attack1")
				{
					m_AControllers[i]->breakable = true;
				}
				else if (m_AControllers[i]->currAnim->GetStridePercent() > 0.85f && m_AControllers[i]->currAnim->GetName() == "Scarlet_Attack2")
				{
					m_AControllers[i]->breakable = true;
				}
				else if (m_AControllers[i]->currAnim->GetStridePercent() > 0.45f && m_AControllers[i]->currAnim->GetName() == "Scarlet_Attack3")
				{
					m_AControllers[i]->breakable = true;
				}
			}

			if (m_AControllers[i]->currAnim->GetIsFinished())
			{
				m_AControllers[i]->breakable = true;
				m_AControllers[i]->isBusy = false;

			}

		}
		else if (m_World->HasComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr))
		{
			if (m_AControllers[i]->breakable)
			{
				auto& enemy = m_World->GetComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr);
				auto& wType = m_World->GetComponent<Frosty::ECS::CEnemy>(m_AControllers[i]->EntityPtr).Weapon->Type;

				if (m_World->HasComponent<Frosty::ECS::CBoss>(m_AControllers[i]->EntityPtr))
				{
					if (enemy.CurrentState == Frosty::ECS::CEnemy::State::Chase || enemy.CurrentState == Frosty::ECS::CEnemy::State::Escape
						|| enemy.CurrentState == Frosty::ECS::CEnemy::State::Reset)
					{
						if (m_AControllers[i]->currAnim->GetName() != "Werewolf_Run")
						{
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Werewolf_Run");
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim->SetIsFinished(false);
							m_AControllers[i]->animSpeed = 1.0;
						}
					}
					else
					{
						if (m_AControllers[i]->currAnim->GetName() != "Werewolf_Idle")
						{
							m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("Werewolf_Idle");
							m_AControllers[i]->currAnim->SetIsRepeating(true);
							m_AControllers[i]->currAnim->SetIsFinished(false);
							m_AControllers[i]->animSpeed = 1.0;
						}
					}
				}
				//If it has bite it is a wolf
				else if (wType == Frosty::ECS::CWeapon::WeaponType::Bite)
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
			else
			{
				if (m_AControllers[i]->currAnim->GetIsFinished())
				{
					m_AControllers[i]->breakable = true;
				}
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

		//Ensure all things with animControllers have them properly mapped in the renderer.
		//If it crashes here please ensure you add Animcontroller component after the mesh.
		Frosty::Renderer::UpdateCMesh(entity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(entity), &m_World->GetComponent<Frosty::ECS::CAnimController>(entity));

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
	auto& it = p_EntityMap.find(entity);

	if (it != p_EntityMap.end())
	{
		auto& world = Frosty::Application::Get().GetWorld();
		Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
		Frosty::ECS::CAnimController* animPtr = world->GetComponentAddress<Frosty::ECS::CAnimController>(entity);

		m_Transform[it->second] = transformPtr;
		m_AControllers[it->second] = animPtr;
		//Special for animation system:
		Frosty::Renderer::UpdateCMesh(entity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(entity), &m_World->GetComponent<Frosty::ECS::CAnimController>(entity));
	}
}

void MCS::AnimationSystem::Render()
{
	//for (size_t i = 1; i < p_Total; i++)
	//{
	//	auto& mesh = m_World->GetComponent<Frosty::ECS::CMesh>(m_Transform[i]->EntityPtr).Mesh;
	//	auto& material = m_World->GetComponent<Frosty::ECS::CMaterial>(m_Transform[i]->EntityPtr);

	//	material.DiffuseTexture->Bind(0);
	//	material.NormalTexture->Bind(1);
	///*	material.SpecularTexture->Bind(2);*/

	//	Frosty::Renderer::AnimSubmit(&material, mesh, m_Transform[i]->ModelMatrix, m_AControllers[i]);

	//	material.DiffuseTexture->Unbind();
	//	material.NormalTexture->Unbind();
	//	//material.SpecularTexture->Unbind();
	//}
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
	if (controller->breakable)
	{
		controller->currAnim = Frosty::AssetManager::GetAnimation("Scarlet_Dash");
		controller->dt = 0;
		/*glm::translate(*controller->currAnim->getHoldingJoint(), glm::vec3(-1.0f, 0.0f, 0.0f));*/
		UpdateAnimOffset(controller);
	}
}

void MCS::AnimationSystem::OnPlayAnimEvent(Frosty::PlayAnimEvent& e)
{
	if (m_World->HasComponent<Frosty::ECS::CAnimController>(e.GetEntity()))
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
				controller->animSpeed = 1.5f;
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

			if (m_World->HasComponent<Frosty::ECS::CBoss>(e.GetEntity()))
			{
				switch (*id)
				{
				case 0:
					BeginNewAnim(controller, "Werewolf_Death");
					controller->animSpeed = 1.0f;
					break;
				case 1:
					BeginNewAnim(controller, "Werewolf_Attack1");
					controller->animSpeed = 1.0f;
					break;
				case 2:
					//Leap
					BeginNewAnim(controller, "Werewolf_Attack2");
					controller->animSpeed = 1.5f;
					break;
				case 3:
					BeginNewAnim(controller, "Werewolf_Charge_Prepare");
					controller->animSpeed = 2.0f;
					break;
				case 4:
					BeginNewAnim(controller, "Werewolf_Run");
					controller->animSpeed = 2.0f;
					controller->currAnim->SetIsRepeating(true);
					controller->breakable = false;
				case 5:
					controller->animSpeed = 1.0f;

				}
			}
			//If it has bite it is a wolf
			else if (wType == Frosty::ECS::CWeapon::WeaponType::Bite)
			{
				switch (*id)
				{
				case 0:
					BeginNewAnim(controller, "Wolf_Death");
					controller->animSpeed = 1.0f;
					break;
				case 1:
					BeginNewAnim(controller, "Wolf_Attack");
					controller->animSpeed = 1.0f;
					break;
				}
			}
			else
			{
				//Otherwise it is cultist.
				switch (*id)
				{
				case 0:
					BeginNewAnim(controller, "Cultist_Death");
					controller->animSpeed = 1.0f;
					break;
				case 1:
					BeginNewAnim(controller, "Cultist_Attack1");
					controller->animSpeed = 3.0f;
					break;
				case 2:
					BeginNewAnim(controller, "Cultist_Attack2");
					controller->animSpeed = 1.8f;
					break;
				}
				UpdateAnimOffset(controller);
			}
		}
	}
	else
	{
		FY_FATAL("AN ANIM EVENT WAS PUBLISHED ON ENTITY: {0}" ,e.GetEntity()->Id);
		FY_FATAL("WITHOUT HAVING AN ANIMCONTROLLER COMPONENT! ");
	}
}

// Resets the time for the anim controller and sets the animation to not repeat and resets the animation.
void MCS::AnimationSystem::BeginNewAnim(Frosty::ECS::CAnimController* controller, const std::string& animName)
{
	controller->dt = 0;
	controller->currAnim = Frosty::AssetManager::GetAnimation(animName);
	controller->currAnim->SetIsRepeating(false);
	controller->currAnim->SetIsFinished(false);
	controller->currAnim->SetStridePercent(0.0f);
	controller->breakable = false;
	controller->isBusy = true;
}

void MCS::AnimationSystem::UpdateAnimOffset(Frosty::ECS::CAnimController* ctrl)
{
	/*ctrl->EntityPtr*/
	if (m_World->HasComponent<Frosty::ECS::CPlayer>(ctrl->EntityPtr))
	{
		auto& wEntity = m_World->GetComponent<Frosty::ECS::CPlayer>(ctrl->EntityPtr).Weapon->EntityPtr;

		m_World->GetComponent<Frosty::ECS::CMesh>(wEntity).animOffset = ctrl->currAnim->getHoldingJoint();
		Frosty::Renderer::UpdateCMesh((int)wEntity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(wEntity));
	}
	else
	{
		//Is cultist
		/*auto& wEntity = m_World->GetComponent<Frosty::ECS::CEnemy>(ctrl->EntityPtr).Weapon->EntityPtr;

		m_World->GetComponent<Frosty::ECS::CMesh>(wEntity).animOffset = ctrl->currAnim->getHoldingJoint();
		Frosty::Renderer::UpdateCMesh(wEntity->Id, &m_World->GetComponent<Frosty::ECS::CMesh>(wEntity));*/
	}
}

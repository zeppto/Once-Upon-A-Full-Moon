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
		if (m_Player[i])
		{
			if (!m_Dash[i]->Active)
			{
				if (m_Physics[i]->Direction.x != 0.0f || m_Physics[i]->Direction.y != 0.0f || m_Physics[i]->Direction.z != 0.0f)
				{
					//m_AControllers[i]->currAnim = Frosty::AssetManager::GetAnimation("NewRun");
				}
				else
				{

				}
			}
			if (m_AControllers[i]->holdPtr != nullptr)
			{
				auto& weapPos = m_World->GetComponent<Frosty::ECS::CTransform>(m_Player[i]->Weapon->EntityPtr);
				auto& playerPos = m_World->GetComponent<Frosty::ECS::CTransform>(m_Player[i]->EntityPtr);

				//glm::vec3 scale;
				//glm::vec3 translation;
				//glm::vec3 skew;
				//glm::vec4 perspective;
				//glm::decompose(*m_AControllers[i]->holdPtr, scale, rotation, translation, skew, perspective);
				//glm::mat4 space = *weapPos.GetModelMatrix() * *playerPos.GetModelMatrix();

				//weapPos.Position = glm::vec3(2.0f,0.0f,0.0f);

				//glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
				//transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
				//transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
				//transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
				//transform = glm::scale(transform, m_Transform[i]->Scale);
				//weapPos.Rotation = glm::eulerAngles(rotation);

				/*weapPos.r*/

				//Parent it.
				m_World->GetComponent<Frosty::ECS::CMesh>(m_Player[i]->Weapon->EntityPtr).parentMatrix = m_Transform[i]->GetModelMatrix();
				m_World->GetComponent<Frosty::ECS::CMesh>(m_Player[i]->Weapon->EntityPtr).animOffset = m_AControllers[i]->holdPtr;

		/*		glm::mat4 final = transform * *weapPos.GetModelMatrix();*/

				/*glm::decompose(final, scale, rotation, translation, skew, perspective);*/

			/*	weapPos.Position = translation;
				weapPos.Rotation = glm::eulerAngles(rotation);
				weapPos.Scale = scale;*/

			/*	m_Transform[i]->ModelMatrix = final;*/
				//weapPos.Position = glm::vec3(0.0f, 2.0f, 0.0f);
				//weapPos.Rotation = glm::vec3(0.0f, 2.0f, 0.0f) * playerPos.Rotation;

			/*	weapPos.Position = translation;
				weapPos.Rotation = */

				/*glm::vec3 pivot = weapPos.Position * playerPos.Position * glm::vec3(-0.74f, 2.14f, 0.13f);
				
				weapPos.Position = ;*/
				/*weapPos.Rotation = playerPos.Rotation;*/
				/*weapPos.Scale = playerPos.Scale; */
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
		m_Player[p_Total] = &world->GetComponent<Frosty::ECS::CPlayer>(entity);

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

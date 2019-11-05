#include <mcspch.hpp>
//#include "EnemyAttackSystem.hpp"
//
//void EnemyAttackSystem::Init()
//{
//	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
//	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
//	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CEnemyAttack>(), true);
//	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
//}
//
//void EnemyAttackSystem::OnInput()
//{
//
//}
//
//void EnemyAttackSystem::OnUpdate()
//{
//	for (size_t i = 1; i < p_Total; i++)
//	{
//		bool haveHit = false;
//		m_EnemyAttack[i]->Cooldown += Frosty::Time::DeltaTime();
//
//		for (size_t j = 1; j < p_Total; j++)
//		{
//			if (j != i && m_EnemyAttack[i]->IsPlayer && m_EnemyAttack[i]->Cooldown > 0.30f && m_Collision[j]->EntityPtr != nullptr)
//			{
//				//glm::mat4 rotationMat(1.0f);
//				//rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
//				//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + m_Collision[i]->BoundingBox->halfSize[0]));
//				//glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);
//
//				glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
//				glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
//				glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
//				glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
//
//				if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
//				{
//					FY_TRACE("EnemyAttack ({0})", j);
//					m_Health[i]->CurrentHealth -= m_EnemyAttack[j]->Damage / 10;
//					FY_TRACE("Player health ({0})", m_Health[i]->CurrentHealth);
//					haveHit = true;
//
//					glm::vec3 knockback = m_Transform[j]->Position - m_Transform[i]->Position;
//					m_Transform[i]->Position.x -= glm::normalize(knockback).x * 4.0f;
//					m_Transform[i]->Position.z -= glm::normalize(knockback).z * 4.0f;
//				}
//			}
//			else if (m_EnemyAttack[i]->Cooldown < 0.20f)
//			{
//				m_EnemyAttack[i]->Cooldown += Frosty::Time::DeltaTime();
//			}
//		}
//		if (haveHit)
//		{
//			m_EnemyAttack[i]->Cooldown = 0;
//		}
//	}
//}
//
//void EnemyAttackSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
//{
//	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
//	{
//		p_EntityMap.emplace(entity, p_Total);
//
//		auto& world = Frosty::Application::Get().GetWorld();
//		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
//		m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
//		m_EnemyAttack[p_Total] = &world->GetComponent<Frosty::ECS::CEnemyAttack>(entity);
//		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
//
//		p_Total++;
//	}
//}
//
//void EnemyAttackSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
//{
//	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];
//
//	if (tempIndex > 0)
//	{
//		p_Total--;
//		m_Transform[p_Total] = nullptr;
//		m_Collision[p_Total] = nullptr;
//		m_EnemyAttack[p_Total] = nullptr;
//		m_Health[p_Total] = nullptr;
//
//		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);
//
//		if (p_Total > tempIndex)
//		{
//			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
//			p_EntityMap[entityToUpdate] = tempIndex;
//		}
//
//		p_EntityMap.erase(entity);
//	}
//}
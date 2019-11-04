#include <fypch.hpp>
#include <mcspch.hpp>
#include "CombatSystem.hpp"

void CombatSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTag>(), true);
	//
	//p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CArrow>(), true);

	if (m_OnlyOneSub)
	{
		m_OnlyOneSub = false;
		Frosty::EventBus::GetEventBus()->Subscribe<CombatSystem, Frosty::BaseEvent>(this, &CombatSystem::OnEvent);
	}
}

void CombatSystem::OnInput()
{
	for (size_t i = 1; i < p_Total; i++)
	{
	}
}

void CombatSystem::OnUpdate()
{
	/*for (size_t i = 1; i < p_Total; i++)
	{
		for (size_t j = 1; j < p_Total; j++)
		{
			if (m_Tag[j]->TagName == "Enemy" && (m_Tag[i]->TagName == "arrow"))
			{

				glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
				glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
				glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;
				glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1], m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
				if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
				{
					if (m_Tag[i]->TagName == "arrow")
					{
						if (m_Arrow[i]->IsPiercing && m_Arrow[i]->alradyHitt == j)
						{
						}
						else
						{
							FY_TRACE("playerAttack ({0})", j);
							m_Health[j]->CurrentHealth -= m_Arrow[i]->Damage;
							m_Health[i]->CurrentHealth--;
							FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
							if (m_Arrow[i]->IsPiercing)
								m_Arrow[i]->alradyHitt = j;
						}
					}

					//nockBack
					//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
					m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 1.0f;
					m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 1.0f;
				}
			}
		}
		//temp death timer
		if (m_Tag[i]->TagName == "arrow")
		{
			m_Arrow[i]->Lifetime--;
			if (m_Arrow[i]->Lifetime < 0)
				m_Health[i]->CurrentHealth--;
		}
	}*/
}

void CombatSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
		m_Tag[p_Total] = &world->GetComponent<Frosty::ECS::CTag>(entity);
		//
		//m_Arrow[p_Total] = &world->GetComponent<Frosty::ECS::CArrow>(entity);

		p_Total++;
	}
}

void CombatSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Collision[p_Total] = nullptr;
		m_Health[p_Total] = nullptr;
		m_Tag[p_Total] = nullptr;
		//
		//m_Arrow[p_Total] = nullptr;

		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}

//temp
void CombatSystem::OnEvent(Frosty::BaseEvent& e)
{
	switch (e.GetEventType())
	{
	//case Frosty::EventType::ArrowHit:
	//	OnArrowHitEvent(static_cast<Frosty::ArrowHitEvent&>(e));
	case Frosty::EventType::NormalHit:
		OnNormalAttackHitEvent(static_cast<Frosty::NormalAttackEvent&>(e));
		break;
	case Frosty::EventType::AreaHit:
		OnAreaAttackHitEvent(static_cast<Frosty::AreaAttackEvent&>(e));
		break;
	case Frosty::EventType::StrongHit:
		OnStrongAttackHitEvent(static_cast<Frosty::StrongAttackEvent&>(e));
		break;
	default:
		break;
	}
}

void CombatSystem::OnNormalAttackHitEvent(Frosty::NormalAttackEvent& e)
{
	FY_TRACE("new ({0})", e.getPlayer()->Id);

	//get player comonents
	auto& world = Frosty::Application::Get().GetWorld();
	auto& playerComp = world->GetComponent<Frosty::ECS::CPlayerAttack>(e.getPlayer());
	auto& transformComp = world->GetComponent<Frosty::ECS::CTransform>(e.getPlayer());
	auto& collisionComp = world->GetComponent<Frosty::ECS::CCollision>(e.getPlayer());

	//creates the offset from player position to create the attack hitbox 
	glm::mat4 rotationMat(1.0f);
	rotationMat = glm::rotate(rotationMat, glm::radians(transformComp.Rotation.y), { 0.0f, 1.0f, 0.0f });
	rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, collisionComp.BoundingBox->halfSize[0] + (collisionComp.BoundingBox->halfSize[0] * playerComp.Reach)));
	glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

	if (playerComp.IsMelee)
	{
		//loop through all entitys the player can hitt
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Tag[i]->TagName != "Player" && Collision(transformComp.Position, transformComp.Scale, collisionComp.BoundingBox->pos, collisionComp.BoundingBox->halfSize,
				m_Transform[i]->Position, m_Transform[i]->Scale, m_Collision[i]->BoundingBox->pos, m_Collision[i]->BoundingBox->halfSize, hitboxPos, playerComp.Reach))
			{
				//the player hitt the enemy
				FY_TRACE("entity ({0}) got hitt", m_Transform[i]->EntityPtr->Id);
				OnHit(&m_Health[i]->CurrentHealth, playerComp.Damage, &m_Transform[i]->Position, transformComp.Position);
			}
		}
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -0.20f;
	}
	else
	{
		//shouts a arrow
		createArrow(transformComp.Position, transformComp.Rotation, glm::vec3(0.9f), hitboxPos, collisionComp.BoundingBox->halfSize[0] * 2.0f * transformComp.Scale);
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -0.40f;
	}
}

void CombatSystem::OnAreaAttackHitEvent(Frosty::AreaAttackEvent& e)
{
	FY_TRACE("new ({0})", e.getPlayer()->Id);

	//get player comonents
	auto& world = Frosty::Application::Get().GetWorld();
	auto& playerComp = world->GetComponent<Frosty::ECS::CPlayerAttack>(e.getPlayer());
	auto& transformComp = world->GetComponent<Frosty::ECS::CTransform>(e.getPlayer());
	auto& collisionComp = world->GetComponent<Frosty::ECS::CCollision>(e.getPlayer());

	if (playerComp.IsMelee)
	{
		//loop through all entitys the player can hitt
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Tag[i]->TagName != "Player" && Collision(transformComp.Position, transformComp.Scale, collisionComp.BoundingBox->pos, collisionComp.BoundingBox->halfSize,
				m_Transform[i]->Position, m_Transform[i]->Scale, m_Collision[i]->BoundingBox->pos, m_Collision[i]->BoundingBox->halfSize, glm::vec3(0.0f), 3.0f))
			{
				//the player hitt the enemy
				FY_TRACE("entity ({0}) got hitt", m_Transform[i]->EntityPtr->Id);
				OnHit(&m_Health[i]->CurrentHealth, playerComp.Damage, &m_Transform[i]->Position, transformComp.Position);
			}
		}
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -0.50f;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			//creates the offset from player position to create tdhe arrows diraction
			glm::mat4 rotationMat(1.0f);
			rotationMat = glm::rotate(rotationMat, glm::radians(transformComp.Rotation.y), { 0.0f, 1.0f, 0.0f });
			if (i == 0)
				rotationMat = glm::rotate(rotationMat, glm::radians(20.0f), { 0.0f, 1.0f, 0.0f });
			if (i == 2)
				rotationMat = glm::rotate(rotationMat, glm::radians(-20.0f), { 0.0f, 1.0f, 0.0f });
			rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, collisionComp.BoundingBox->halfSize[0] + (collisionComp.BoundingBox->halfSize[0] * playerComp.Reach)));
			glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

			//shouts a arrow
			createArrow(transformComp.Position, transformComp.Rotation, glm::vec3(0.9f), hitboxPos, collisionComp.BoundingBox->halfSize[0] * 2.0f * transformComp.Scale);
		}
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -0.80f;
	}
}

void CombatSystem::OnStrongAttackHitEvent(Frosty::StrongAttackEvent& e)



{
	FY_TRACE("new ({0})", e.getPlayer()->Id);

	//get player comonents
	auto& world = Frosty::Application::Get().GetWorld();
	auto& playerComp = world->GetComponent<Frosty::ECS::CPlayerAttack>(e.getPlayer());
	auto& transformComp = world->GetComponent<Frosty::ECS::CTransform>(e.getPlayer());
	auto& collisionComp = world->GetComponent<Frosty::ECS::CCollision>(e.getPlayer());

	//creates the offset from player position to create the attack hitbox 
	glm::mat4 rotationMat(1.0f);
	rotationMat = glm::rotate(rotationMat, glm::radians(transformComp.Rotation.y), { 0.0f, 1.0f, 0.0f });
	rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, collisionComp.BoundingBox->halfSize[0] + (collisionComp.BoundingBox->halfSize[0] * playerComp.Reach)));
	glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

	if (playerComp.IsMelee)
	{
		//loop through all entitys the player can hitt
		for (size_t i = 1; i < p_Total; i++)
		{
			glm::vec3 finalCenterB = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1], m_Collision[i]->BoundingBox->pos[2]);
			glm::vec3 finalLengthB = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0], m_Collision[i]->BoundingBox->halfSize[1],
				m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale;

			glm::vec3 rayLength = -glm::normalize(transformComp.Position - (transformComp.Position + hitboxPos)) * glm::vec3(10);
			if (Frosty::CollisionDetection::AABBRayIntersect(transformComp.Position, rayLength, finalLengthB, finalCenterB))
			{
				//the player hitt the enemy
				FY_TRACE("entity ({0}) got hitt", m_Transform[i]->EntityPtr->Id);
				OnHit(&m_Health[i]->CurrentHealth, playerComp.Damage, &m_Transform[i]->Position, transformComp.Position);
			}
		}
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -1.00f;
	}
	else
	{
		//shouts a arrow
		createArrow(transformComp.Position, transformComp.Rotation, glm::vec3(0.9f), hitboxPos, collisionComp.BoundingBox->halfSize[0] * 2.0f * transformComp.Scale, 3, true);
		//cooldown for the time it is going to take the player to attack again
		playerComp.Cooldown = -1.20f;
	}
}

void CombatSystem::OnHit(float* hitHp, float damage, glm::vec3* hitPos, glm::vec3 attackerPos)
{
	*hitHp -= damage;
	FY_TRACE("current health({0})", *hitHp);
	//nockBack
	//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
	hitPos->x += glm::normalize(*hitPos - attackerPos).x * 4.0f;
	hitPos->z += glm::normalize(*hitPos - attackerPos).z * 4.0f;
}

void CombatSystem::createArrow(glm::vec3 spwanPos, glm::vec3 rotation, glm::vec3 scale, glm::vec3 hitboxPos, 
	glm::vec3 offsetFromspwan, int arrowDamage, bool isPiercing, float lifeTime)
{
	//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
	auto& world = Frosty::Application::Get().GetWorld();
	auto& arrow = world->CreateEntity();
	auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
	arrowTransform.Position = spwanPos - glm::normalize(spwanPos - (spwanPos + hitboxPos)) * offsetFromspwan;
	arrowTransform.Scale *= scale;
	arrowTransform.Rotation = rotation;
	world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
	world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
	auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 40.0f);
	arrowMotion.Direction = -glm::normalize(spwanPos - (spwanPos + hitboxPos));
	arrowMotion.Direction.y = 0;
	FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(spwanPos - (spwanPos + hitboxPos)).x,
		-glm::normalize(spwanPos - (spwanPos + hitboxPos)).y,
		-glm::normalize(spwanPos - (spwanPos + hitboxPos)).z);
	world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
	world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
	auto& arrowArrow = world->AddComponent<Frosty::ECS::CArrow>(arrow);
	arrowArrow.Damage = arrowDamage;
	arrowArrow.IsPiercing = isPiercing;
	arrowArrow.Lifetime = lifeTime;
}

bool CombatSystem::Collision(glm::vec3 posA, glm::vec3 scaleA, float centerA[3], float halfSizeA[3], glm::vec3 posB, glm::vec3 scaleB,
	float centerB[3],float  halfSizeB[3], glm::vec3 hitboxPos, float reach)
{
	glm::vec3 finalCenterA = posA + glm::vec3(centerA[0], centerA[1], centerA[2]) + hitboxPos;
	glm::vec3 finalCenterB = posB + glm::vec3(centerB[0], centerB[1], centerB[2]);
	glm::vec3 finalLengthA = glm::vec3(halfSizeA[0], halfSizeA[1], halfSizeA[2]) * scaleA * reach;
	glm::vec3 finalLengthB = glm::vec3(halfSizeB[0], halfSizeB[1], halfSizeB[2]) * scaleB;
	return Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB);
}

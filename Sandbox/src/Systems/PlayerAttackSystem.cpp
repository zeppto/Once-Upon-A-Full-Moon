#include <fypch.hpp>
#include <mcspch.hpp>
#include "PlayerAttackSystem.hpp"

void PlayerAttackSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCollision>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPlayerAttack>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
}

void PlayerAttackSystem::OnInput()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		//FY_TRACE("playerAttack ({0})", i);

		//tips hoger ar fram utan rotation
		bool hasHitt = false;
		if (m_PlayerAttack[i]->IsPlayer)
		{
			m_PlayerAttack[i]->Cooldown += Frosty::Time::DeltaTime();
			//FY_TRACE("cooldown ({0})", m_PlayerAttack[i]->Cooldown);
			if (Frosty::InputManager::IsKeyReleased(FY_KEY_ENTER))
			{
				m_CanSwitchWepond = true;
			}
			if (Frosty::InputManager::IsKeyPressed(FY_KEY_ENTER) && m_CanSwitchWepond)
			{
				m_CanSwitchWepond = false;
				if (m_PlayerAttack[i]->IsMelee) {
					m_PlayerAttack[i]->IsMelee = false;
					m_PlayerAttack[i]->CurrTexture = 1;

				}
				else {
					m_PlayerAttack[i]->IsMelee = true;
					m_PlayerAttack[i]->CurrTexture = 0;
				}

			}

			if (m_PlayerAttack[i]->IsMelee)
			{
				//attack

				if (Frosty::InputManager::IsMouseButtonReleased(FY_MOUSE_BUTTON_LEFT))
				{
					m_CanAttackNormal = true;
				}
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{
					for (size_t j = 1; j < p_Total; j++)
					{
						if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackNormal && m_Collision[j]->EntityPtr != nullptr)
						{
							hasHitt = true;

							glm::mat4 rotationMat(1.0f);
							rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
							//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
							rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
							glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

							glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1],
								m_Collision[i]->BoundingBox->pos[2]) + hitboxPos;
							glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
							glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach,
								m_Collision[i]->BoundingBox->halfSize[1] * m_PlayerAttack[i]->Reach, m_Collision[i]->BoundingBox->halfSize[2] * m_PlayerAttack[i]->Reach) * m_Transform[i]->Scale;
							glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
								m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
							if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
							{
								FY_TRACE("playerAttack ({0})", j);
								m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage;
								FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);
								//nockBack
								//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
								m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 4.0f;
								m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 4.0f;

							}
						}
					}
					if (hasHitt)
					{
						m_CanAttackNormal = false;
						m_PlayerAttack[i]->Cooldown = -0.10f;
					}
				}
				//area attack 
				if (Frosty::InputManager::IsMouseButtonReleased(FY_MOUSE_BUTTON_RIGHT))
				{
					m_CanAttackArea = true;
				}
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_RIGHT))
				{
					for (size_t j = 1; j < p_Total; j++)
					{
						if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackArea && m_Collision[j]->EntityPtr != nullptr)
						{
							hasHitt = true;

							//temp *2 make it better
							glm::vec3 finalCenterA = m_Transform[i]->Position + glm::vec3(m_Collision[i]->BoundingBox->pos[0], m_Collision[i]->BoundingBox->pos[1],
								m_Collision[i]->BoundingBox->pos[2]);
							glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
							glm::vec3 finalLengthA = glm::vec3(m_Collision[i]->BoundingBox->halfSize[0],
								m_Collision[i]->BoundingBox->halfSize[1], m_Collision[i]->BoundingBox->halfSize[2]) * m_Transform[i]->Scale * glm::vec3(3.00f);
							glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
								m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;
							if (Frosty::CollisionDetection::AABBIntersect(finalLengthA, finalCenterA, finalLengthB, finalCenterB))
							{
								FY_TRACE("playerAttack ({0})", j);
								m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage / 2;
								FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);

								//nockBack
								//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
								m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 5.0f;
								m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 5.0f;

							}
						}
					}
					if (hasHitt)
					{
						m_CanAttackArea = false;
						m_PlayerAttack[i]->Cooldown = -0.50f;
					}
				}
				//strong slim attack 
				if (Frosty::InputManager::IsKeyReleased(FY_KEY_SPACE))
				{
					m_CanAttackStrong = true;
				}
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
				{
					for (size_t j = 1; j < p_Total; j++)
					{
						if (j != i && m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackStrong && m_Collision[j]->EntityPtr != nullptr)
						{
							hasHitt = true;

							glm::mat4 rotationMat(1.0f);
							rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
							rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
							glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

							glm::vec3 finalCenterB = m_Transform[j]->Position + glm::vec3(m_Collision[j]->BoundingBox->pos[0], m_Collision[j]->BoundingBox->pos[1], m_Collision[j]->BoundingBox->pos[2]);
							glm::vec3 finalLengthB = glm::vec3(m_Collision[j]->BoundingBox->halfSize[0], m_Collision[j]->BoundingBox->halfSize[1],
								m_Collision[j]->BoundingBox->halfSize[2]) * m_Transform[j]->Scale;

							glm::vec3 rayLength = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * glm::vec3(10);
							if (Frosty::CollisionDetection::AABBRayIntersect(m_Transform[i]->Position, rayLength, finalLengthB, finalCenterB))
							{
								FY_TRACE("playerAttack ({0})", j);
								m_Health[j]->CurrentHealth -= m_PlayerAttack[i]->Damage * 2;
								FY_TRACE("current health ({0})", m_Health[j]->CurrentHealth);

								//nockBack
								//enemy sak ta hand om sin egen push back, har sak bara skickas till enemyn att du ska push backa
								m_Transform[j]->Position.x -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).x * 4.0f;
								m_Transform[j]->Position.z -= glm::normalize(m_Transform[i]->Position - m_Transform[j]->Position).z * 4.0f;
							}
						}
					}
					if (hasHitt)
					{
						m_CanAttackStrong = false;
						m_PlayerAttack[i]->Cooldown = -1.00f;
					}
				}
			}
			else
			{
				//arrow
				if (Frosty::InputManager::IsMouseButtonReleased(FY_MOUSE_BUTTON_LEFT))
				{
					m_CanAttackNormal = true;
				}
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_LEFT))
				{

					if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackNormal)
					{
						m_CanAttackNormal = false;
						m_PlayerAttack[i]->Cooldown = -0.40f;

						glm::mat4 rotationMat(1.0f);
						rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
						//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
						rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
						glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

						//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
						auto& world = Frosty::Application::Get().GetWorld();
						auto& arrow = world->CreateEntity();
						auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
						arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
						arrowTransform.Scale *= 0.9f;
						arrowTransform.Rotation = m_Transform[i]->Rotation;
						world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
						world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
						auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 40.0f);
						arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
						arrowMotion.Direction.y = 0;
						FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
						world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
						world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
						world->AddComponent<Frosty::ECS::CHealth>(arrow, 1.0f);
						world->AddComponent<Frosty::ECS::CArrow>(arrow);

					}
				}
				//3 arrows
				if (Frosty::InputManager::IsMouseButtonReleased(FY_MOUSE_BUTTON_RIGHT))
				{
					m_CanAttackArea = true;
				}
				if (Frosty::InputManager::IsMouseButtonPressed(FY_MOUSE_BUTTON_RIGHT))
				{
					if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackArea)
					{
						m_CanAttackArea = false;
						m_PlayerAttack[i]->Cooldown = -0.80f;

						for (int k = 0; k < 3; k++)
						{
							glm::mat4 rotationMat(1.0f);
							rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
							if (k == 0)
								rotationMat = glm::rotate(rotationMat, glm::radians(20.0f), { 0.0f, 1.0f, 0.0f });
							if (k == 2)
								rotationMat = glm::rotate(rotationMat, glm::radians(-20.0f), { 0.0f, 1.0f, 0.0f });
							//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
							rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
							glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

							//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
							auto& world = Frosty::Application::Get().GetWorld();
							auto& arrow = world->CreateEntity();
							auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
							arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
							arrowTransform.Scale *= 0.9f;
							arrowTransform.Rotation = m_Transform[i]->Rotation;
							world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
							world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
							auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 40.0f);
							arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
							arrowMotion.Direction.y = 0;
							FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
							world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
							world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
							world->AddComponent<Frosty::ECS::CHealth>(arrow, 1.0f);
							world->AddComponent<Frosty::ECS::CArrow>(arrow);
						}

					}
					m_CanAttackArea = false;
				}
				//strong arrow
				if (Frosty::InputManager::IsKeyReleased(FY_KEY_SPACE))
				{
					m_CanAttackStrong = true;
				}
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_SPACE))
				{
					if (m_PlayerAttack[i]->IsPlayer && m_PlayerAttack[i]->Cooldown > 0.00f && m_CanAttackStrong)
					{
						m_CanAttackStrong = false;
						m_PlayerAttack[i]->Cooldown = -1.40f;

						glm::mat4 rotationMat(1.0f);
						rotationMat = glm::rotate(rotationMat, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
						//rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_PlayerAttack[i]->Reach / 2)));
						rotationMat = glm::translate(rotationMat, glm::vec3(0, 0, m_Collision[i]->BoundingBox->halfSize[0] + (m_Collision[i]->BoundingBox->halfSize[0] * m_PlayerAttack[i]->Reach)));
						glm::vec3 hitboxPos = glm::vec3(rotationMat[3]);

						//test --- to create arrow (do a call to an other funktinon insted that adds and removes arrows)
						auto& world = Frosty::Application::Get().GetWorld();
						auto& arrow = world->CreateEntity();
						auto& arrowTransform = world->GetComponent<Frosty::ECS::CTransform>(arrow);
						arrowTransform.Position = m_Transform[i]->Position - glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)) * m_Collision[i]->BoundingBox->halfSize[0] * 2.0f * m_Transform[i]->Scale;
						arrowTransform.Scale *= 0.9f;
						arrowTransform.Rotation = m_Transform[i]->Rotation;
						world->AddComponent<Frosty::ECS::CMesh>(arrow, Frosty::AssetManager::GetMesh("Cube"));
						world->AddComponent<Frosty::ECS::CMaterial>(arrow, Frosty::AssetManager::GetShader("FlatColor"));
						auto& arrowMotion = world->AddComponent<Frosty::ECS::CMotion>(arrow, 40.0f);
						arrowMotion.Direction = -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos));
						arrowMotion.Direction.y = 0;
						FY_TRACE("playerAttack Direction({0}, {1}, {2})", -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).x, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).y, -glm::normalize(m_Transform[i]->Position - (m_Transform[i]->Position + hitboxPos)).z);
						world->AddComponent<Frosty::ECS::CTag>(arrow, "arrow");
						world->AddComponent<Frosty::ECS::CCollision>(arrow, Frosty::AssetManager::GetBoundingBox("Cube"));
						world->AddComponent<Frosty::ECS::CHealth>(arrow, 10.0f);
						world->AddComponent<Frosty::ECS::CArrow>(arrow, 3.0f, 50, true);

					}
					m_CanAttackArea = false;
				}
			}
		}
	}
}

void PlayerAttackSystem::OnUpdate()
{
	for (size_t i = 1; i < p_Total; i++)
	{
	}
}

void PlayerAttackSystem::Render()
{
	for (size_t i = 1; i < p_Total; i++)
	{

		if (m_PlayerAttack[i]->Texture[m_PlayerAttack[i]->CurrTexture] && m_PlayerAttack[i]->UseShader->GetName() == "UI")
			m_PlayerAttack[i]->Texture[m_PlayerAttack[i]->CurrTexture]->Bind(0);

		Frosty::Renderer::Submit2d(m_PlayerAttack[i]->Texture[m_PlayerAttack[i]->CurrTexture].get(), m_PlayerAttack[i]->UseShader.get(), m_PlayerAttack[i]->Mesh, m_PlayerAttack[i]->TextureTransform);

		if (m_PlayerAttack[i]->UseShader->GetName() == "UI" && m_PlayerAttack[i]->Texture[m_PlayerAttack[i]->CurrTexture])
			m_PlayerAttack[i]->Texture[m_PlayerAttack[i]->CurrTexture]->Unbind();
	}
}

void PlayerAttackSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Collision[p_Total] = &world->GetComponent<Frosty::ECS::CCollision>(entity);
		m_PlayerAttack[p_Total] = &world->GetComponent<Frosty::ECS::CPlayerAttack>(entity);
		m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);

		if (!m_PlayerAttack[p_Total]->Mesh)
		{
			m_PlayerAttack[p_Total]->Mesh = Frosty::AssetManager::GetMesh("Plane");
			m_PlayerAttack[p_Total]->UseShader = Frosty::AssetManager::GetShader("UI");
			m_PlayerAttack[p_Total]->Texture[0] = Frosty::AssetManager::GetTexture2D("Sword");
			m_PlayerAttack[p_Total]->Texture[1] = Frosty::AssetManager::GetTexture2D("Bow");
		}

		m_PlayerAttack[p_Total]->TextureTransform = glm::translate(m_PlayerAttack[p_Total]->TextureTransform, glm::vec3(-0.75f, -0.75f, 0.0f));
		m_PlayerAttack[p_Total]->TextureTransform = glm::scale(m_PlayerAttack[p_Total]->TextureTransform, glm::vec3(0.25f));
		m_PlayerAttack[p_Total]->TextureTransform = glm::rotate(m_PlayerAttack[p_Total]->TextureTransform, glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });


		p_Total++;
	}
}

void PlayerAttackSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Collision[p_Total] = nullptr;
		m_PlayerAttack[p_Total] = nullptr;
		m_Health[p_Total] = nullptr;

		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}
#include <mcspch.hpp>
#include "PlayerControllerSystem.hpp"

void PlayerControllerSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CController>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMotion>(), true);
}

void PlayerControllerSystem::OnInput()
{
	for (size_t i = 1; i < p_Total; i++)
	{
		if (m_Motion[i]->DashCurrentCooldown > 0.0) m_Motion[i]->DashCurrentCooldown -= Frosty::Time::DeltaTime();

		// Check dash status
		if (!m_Motion[i]->DashActive)
		{
			// Reset
			m_Motion[i]->Velocity = glm::vec3(0.0f);

			// Get the point on our terrain
			glm::vec3 point3D = ScreenToTerrainPoint();

			// Look at point
			LookAtPoint(point3D, i);

			// Calculate direction based on rotation
			//CalculateDirection(i);

			m_Motion[i]->Direction = glm::vec3(0.0f, 0.0f, 0.0f);
			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveForwardKey))
			{
				m_Motion[i]->Direction.z = -1.0f;
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
				{
					m_Motion[i]->DashActive = true;
				}
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveBackKey))
			{
				m_Motion[i]->Direction.z = 1.0f;
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
				{
					m_Motion[i]->DashActive = true;
				}
			}
			if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveLeftKey))
			{
				m_Motion[i]->Direction.x = -1.0f;
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
				{
					m_Motion[i]->DashActive = true;
				}
			}
			else if (Frosty::InputManager::IsKeyPressed(m_Controller[i]->MoveRightKey))
			{
				m_Motion[i]->Direction.x = 1.0f;
				if (Frosty::InputManager::IsKeyPressed(FY_KEY_LEFT_SHIFT) && m_Motion[i]->DashCurrentCooldown <= 0.0f)
				{
					m_Motion[i]->DashActive = true;
				}
			}

			if (glm::length(m_Motion[i]->Direction) > 0.0f)
			{
				m_Motion[i]->Velocity = m_Motion[i]->Direction * m_Motion[i]->Speed;
				if (m_Motion[i]->DashActive)
				{
					m_Motion[i]->Velocity *= m_Motion[i]->DashSpeedMultiplier;
					m_Motion[i]->DashCurrentCooldown = m_Motion[i]->DASH_COOLDOWN / 1000.0f;
				}
			}
		}
	}
}

void PlayerControllerSystem::OnUpdate()
{

}

void PlayerControllerSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Controller[p_Total] = &world->GetComponent<Frosty::ECS::CController>(entity);
		m_Motion[p_Total] = &world->GetComponent<Frosty::ECS::CMotion>(entity);

		p_Total++;
	}
}

void PlayerControllerSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Controller[p_Total] = nullptr;
		m_Motion[p_Total] = nullptr;

		if (p_Total > 1)
		{
			//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

			if (p_Total > tempIndex)
			{
				std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Controller[p_EntityMap[entity]]->EntityPtr;
				p_EntityMap[entityToUpdate] = tempIndex;
			}
		}

		p_EntityMap.erase(entity);
	}
}

glm::vec3 PlayerControllerSystem::ScreenToTerrainPoint()
{
	glm::vec4 viewport = Frosty::Application::Get().GetWindow().GetViewport();
	auto& gameCameraEntity = Frosty::Application::Get().GetWorld()->GetSceneCamera();
	auto& gameCameraTransformComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CTransform>(gameCameraEntity);
	auto& gameCameraCamerComp = Frosty::Application::Get().GetWorld()->GetComponent<Frosty::ECS::CCamera>(gameCameraEntity);
	glm::vec2 mousePos = glm::vec2(Frosty::InputManager::GetMouseX(), Frosty::InputManager::GetMouseY());


	// Convert from viewport to NDC
	glm::vec2 NDC = glm::vec2(
		(2.0f * (mousePos.x - viewport.x)) / viewport.z - 1.0f,
		(2.0f * (mousePos.y - viewport.y)) / viewport.w - 1.0f
	);

	// Convert from NDC to clip
	glm::vec4 clipRayCoords = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);

	// Convert from clip to eye
	glm::vec4 eyeRayCoords = glm::inverse(gameCameraCamerComp.ProjectionMatrix) * clipRayCoords;
	eyeRayCoords.z = -1.0f;
	eyeRayCoords.w = 0.0f;

	// Convert from eye to world (and then normalize)
	glm::vec4 worldRayCoords = glm::inverse(gameCameraCamerComp.ViewMatrix) * eyeRayCoords;
	glm::vec3 mouseWorldRay = normalize(glm::vec3(worldRayCoords));

	// Now we got a normalized vector from our screen position. Use this to find point in 3D space
	float div = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), mouseWorldRay);
	float t = (1.0f - glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), gameCameraTransformComp.Position)) / div;
	glm::vec3 point3D = t * mouseWorldRay + gameCameraTransformComp.Position;

	return point3D;
}

void PlayerControllerSystem::LookAtPoint(const glm::vec3& point, size_t index)
{
	// Rotate the player to look towards the mouse (point3D)
	glm::vec3 pointVector = glm::normalize(point - m_Transform[index]->Position);
	glm::vec3 originDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	float extraRotation = 0.0f;
	if (point.x <= m_Transform[index]->Position.x)
	{
		originDirection.z = -1.0f;
		extraRotation = 180.0f;
	}
	float product = glm::dot(glm::normalize(originDirection), pointVector);

	float rotationOffset = glm::degrees(glm::acos(product)) + extraRotation;


	m_Transform[index]->Rotation.y = rotationOffset;
}

void PlayerControllerSystem::CalculateDirection(size_t index)
{
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.x), { 1.0f, 0.0f, 0.0f });
	mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.y), { 0.0f, 1.0f, 0.0f });
	mat = glm::rotate(mat, glm::radians(m_Transform[index]->Rotation.z), { 0.0f, 0.0f, 1.0f });

	m_Motion[index]->Direction = glm::normalize(mat * glm::vec4(0.0f, 0.0f, 1.0f, 0.0));
}
#include <mcspch.hpp>
#include "CameraSystem.hpp"

void CameraSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CCamera>(), true);

	//Needed for testing
	//Frosty::PrefabManager::GetPrefabManager()->SetPrefab("StorKlocka", "klocka","Mat_0:newClock");
}

void CameraSystem::OnUpdate()
{
	auto& win = Frosty::Application::Get().GetWindow();
	glm::vec3 front;
	for (size_t i = 1; i < p_Total; i++)
	{
		// Follow player
		if (m_Cameras[i]->Target)
		{
			m_Transform[i]->Position = m_Cameras[i]->Target->Position + glm::vec3(0.0f, 30.0f, 30.0f);
			m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, m_Cameras[i]->Target->Position, { 0.0f, 1.0f, 0.0f });
		}
		else
		{
			front.x = cos(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
			front.y = sin(glm::radians(m_Transform[i]->Rotation.y));
			front.z = sin(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
			m_Cameras[i]->Front = glm::normalize(front);

			m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, m_Transform[i]->Position + m_Cameras[i]->Front, { 0.0f, 1.0f, 0.0f });
		}

		m_Cameras[i]->ViewProjectionMatrix = m_Cameras[i]->ProjectionMatrix * m_Cameras[i]->ViewMatrix;
	}
}

void CameraSystem::BeginScene()
{
	if (p_Total > 1)
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			Frosty::RenderCommand::SetClearColor(glm::vec4(m_Cameras[i]->Background, 1.0f));
			Frosty::RenderCommand::Clear();
			Frosty::Renderer::BeginScene();
			Frosty::Renderer::SetCamera(m_Transform[i]->Position, m_Cameras[i]->ViewMatrix, m_Cameras[i]->ProjectionMatrix);
		}
	}
	else
	{
		Frosty::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Frosty::RenderCommand::Clear();
		Frosty::Renderer::BeginScene();
		Frosty::Renderer::SetCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::mat4(1.0f), glm::mat4(1.0f));
	}
}

void CameraSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_Cameras[p_Total] = &world->GetComponent<Frosty::ECS::CCamera>(entity);

		// Initiate entity based stuff here
		auto& win = Frosty::Application::Get().GetWindow();
		glm::vec3 front;
		front.x = cos(glm::radians(m_Transform[p_Total]->Rotation.x)) * cos(glm::radians(m_Transform[p_Total]->Rotation.y));
		front.y = sin(glm::radians(m_Transform[p_Total]->Rotation.y));
		front.z = sin(glm::radians(m_Transform[p_Total]->Rotation.x)) * cos(glm::radians(m_Transform[p_Total]->Rotation.y));
		m_Cameras[p_Total]->Front = glm::normalize(front);

		m_Cameras[p_Total]->ProjectionMatrix = glm::perspective(glm::radians(m_Cameras[p_Total]->FieldOfView), (float)(win.GetViewport().z / win.GetViewport().w), m_Cameras[p_Total]->Near, m_Cameras[p_Total]->Far);
		m_Cameras[p_Total]->ViewMatrix = glm::lookAt(m_Transform[p_Total]->Position, m_Transform[p_Total]->Position + m_Cameras[p_Total]->Front, { 0.0f, 1.0f, 0.0f });
		m_Cameras[p_Total]->ViewProjectionMatrix = m_Cameras[p_Total]->ProjectionMatrix * m_Cameras[p_Total]->ViewMatrix;

		p_Total++;
	}
}

void CameraSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Cameras[p_Total] = nullptr;

		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}
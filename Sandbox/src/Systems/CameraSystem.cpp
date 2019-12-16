#include <mcspch.hpp>
#include "CameraSystem.hpp"

namespace MCS
{
	const std::string CameraSystem::NAME = "Camera";

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
				//m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, glm::vec3(0.0f, 0.0f, 0.0f), { 0.0f, 1.0f, 0.0f });
				m_Transform[i]->Position = m_Cameras[i]->Target->Position + glm::vec3(0.0f, 40.0f, 15.0f);
				m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, m_Cameras[i]->Target->Position, { 0.0f, 1.0f, 0.0f });
			}
			else
			{
				//m_Transform[i]->Position = glm::vec3(0.0f, 125.0f, 0.1f);
				//m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, glm::vec3(0.0f, 0.0f, 0.0f), { 0.0f, 1.0f, 0.0f });
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
		Frosty::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Frosty::RenderCommand::Clear();
		Frosty::Renderer::BeginScene();
		Frosty::Renderer::SetCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::mat4(1.0f), glm::mat4(1.0f));

		for (size_t i = 1; i < p_Total; i++)
		{
			Frosty::RenderCommand::SetClearColor(glm::vec4(m_Cameras[i]->Background, 1.0f));
			Frosty::RenderCommand::Clear();
			//Frosty::Renderer::BeginScene();
			Frosty::Renderer::SetCamera(m_Transform[i]->Position, m_Cameras[i]->ViewMatrix, m_Cameras[i]->ProjectionMatrix);
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
		auto& it = p_EntityMap.find(entity);
		
		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Cameras[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void CameraSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CCamera* cameraPtr = world->GetComponentAddress<Frosty::ECS::CCamera>(entity);

			m_Transform[it->second] = transformPtr;
			m_Cameras[it->second] = cameraPtr;
		}
	}
	
	std::string CameraSystem::GetInfo() const
	{
		std::stringstream retInfo;
		retInfo << "\t-----------" << NAME << " System Info-----------\n";
		retInfo << "\t\t---------Entity Map---------\n";
		retInfo << "\t\tEntity Id\tEntity Address\t\tEntity Refs\tArray Index\n";
		for (auto& em : p_EntityMap)
		{
			retInfo << "\t\t" << em.first->Id << "\t\t" << em.first << "\t\t" << em.first.use_count() << "\t" << em.second << "\n";
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t\t------Component Array(s)------\n";
		retInfo << "\n\t\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < p_Total; i++)
		{
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Cameras[i] << "\t" << m_Cameras[i]->EntityPtr->Id << "\t\t" << m_Cameras[i]->EntityPtr << "\t\t" << m_Cameras[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}
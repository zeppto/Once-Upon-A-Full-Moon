#include <fypch.hpp>
#include "ECS.hpp"
#include "Frosty/Core/Application.hpp"
#include "Frosty/Core/InputManager.hpp"

#include <GLFW/glfw3.h>

namespace Frosty
{
	namespace ECS
	{
		std::string CTransform::NAME = "Transform";
		std::string CMesh::NAME = "Mesh";
		std::string CCamera::NAME = "Camera";
		std::string CMaterial::NAME = "Material";
		/*
		void CameraSystem::OnInput()
		{
			float velocity = 1.0f * Time::DeltaTime();

			for (size_t i = 1; i < p_Total; i++)
			{
				if (InputManager::IsKeyPressed(GLFW_KEY_D))
				{
					// Move Right
					m_Transform[i]->Position += glm::cross(m_Cameras[i]->Front, { 0.0f, 1.0f, 0.0f }) * velocity;
				}
				else if (InputManager::IsKeyPressed(GLFW_KEY_A))
				{
					// Move Right
					m_Transform[i]->Position += glm::cross(m_Cameras[i]->Front, { 0.0f, 1.0f, 0.0f }) * velocity * -1.0f;
				}
			}
		}

		void CameraSystem::OnUpdate()
		{
			auto& win = Application::Get().GetWindow();
			glm::vec3 front;
			for (size_t i = 1; i < p_Total; i++)
			{
				front.x = cos(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
				front.y = sin(glm::radians(m_Transform[i]->Rotation.y));
				front.z = sin(glm::radians(m_Transform[i]->Rotation.x)) * cos(glm::radians(m_Transform[i]->Rotation.y));
				m_Cameras[i]->Front = glm::normalize(front);

				m_Cameras[i]->ViewMatrix = glm::lookAt(m_Transform[i]->Position, m_Transform[i]->Position + m_Cameras[i]->Front, { 0.0f, 1.0f, 0.0f });
				m_Cameras[i]->ViewProjectionMatrix = m_Cameras[i]->ProjectionMatrix * m_Cameras[i]->ViewMatrix;
			}
		}

		void CameraSystem::AddComponent(const std::shared_ptr<Entity>& entity)
		{
			if (utils::BitsetFits<MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
			{
				p_EntityMap.emplace(entity, p_Total);

				auto& world = Application::Get().GetWorld();
				m_Transform[p_Total] = &world->GetComponent<CTransform>(entity);
				m_Cameras[p_Total] = &world->GetComponent<CCamera>(entity);

				p_Total++;
			}
		}
		
		void RenderSystem::AddComponent(const std::shared_ptr<Entity>& entity)
		{
			if (utils::BitsetFits<MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
			{
				p_EntityMap.emplace(entity, p_Total);

				auto& world = Application::Get().GetWorld();
				m_Transform[p_Total] = &world->GetComponent<CTransform>(entity);
				m_Meshes[p_Total] = &world->GetComponent<CMesh>(entity);
				m_Materials[p_Total] = &world->GetComponent<CMaterial>(entity);

				p_Total++;
			}
		}
		*/
	}
}
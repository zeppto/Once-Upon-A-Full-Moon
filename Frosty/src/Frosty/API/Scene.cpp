#include <fypch.hpp>
#include "Scene.hpp"

#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/API/AssetManager.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	void Scene::Init()
	{
		// Initiate entity manager with a fixed size for all scenes or user defined for each specific scene?
		m_EntityManager.reset(FY_NEW ECS::EntityManager());
	}

	void Scene::AddCamera(std::shared_ptr<ECS::Entity>& camera)
	{
		m_GameCamera = camera;
	}

	std::shared_ptr<ECS::Entity>& Scene::CreateEntity(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		auto& entity = m_EntityManager->Create();

		AddComponent<ECS::CTransform>(entity, position, rotation, scale);

		return entity;
	}

	void Scene::RemoveEntity(std::shared_ptr<ECS::Entity>& entity)
	{
		for (size_t i = 0; i < entity->Bitset.size(); i++)
		{
			if (entity->Bitset[i])
			{
				m_ComponentManagers[i]->Remove(entity);
			}
		}

		m_EntityManager->Remove(entity);
	}
}
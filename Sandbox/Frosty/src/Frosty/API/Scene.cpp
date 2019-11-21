#include <fypch.hpp>
#include "Scene.hpp"

#include "Frosty/RenderEngine/Renderer.hpp"
#include"Frosty/API/AssetManager/AssetManager.hpp"
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

	std::shared_ptr<ECS::Entity>& Scene::CreateEntity(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool isStatic)
	{
		auto& entity = m_EntityManager->Create();

		AddComponent<ECS::CTransform>(entity, position, rotation, scale, isStatic);

		return entity;
	}

	void Scene::RemoveEntity(const std::shared_ptr<ECS::Entity>& entity)
	{
		for (size_t i = 0; i < entity->Bitset.size(); i++)
		{
			if (entity->Bitset[i])
			{
				auto& entityToUpdate = m_ComponentManagers[i]->Remove(entity);
				auto& world = Application::Get().GetWorld();
				world->UpdateSystems(entityToUpdate);
			}
		}

		m_EntityManager->Remove(entity);
	}
	
	void Scene::PrintScene()
	{
		FY_CORE_INFO("\t\t=======STARTING TO PRINT WORLD INFO=======\n{0}", *m_EntityManager);

		for (size_t i = 0; i < ECS::MAX_COMPONENTS; i++)
		{
			if (m_ComponentManagers[i])
			{
				FY_CORE_INFO("\n{0}", *m_ComponentManagers[i]);
			}
		}
	}
}
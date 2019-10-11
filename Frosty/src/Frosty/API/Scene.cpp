#include <fypch.hpp>
#include "Scene.hpp"

#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/API/AssetManager.hpp"


namespace Frosty
{
	Scene::Scene()
	{

	}

	void Scene::Init()
	{
		// Initiate entity manager with a fixed size for all scenes or user defined for each specific scene?
		m_EntityManager.reset(FY_NEW ECS::EntityManager());
	}

	std::shared_ptr<ECS::Entity>& Scene::CreateEntity()
	{
		auto& entity = m_EntityManager->Create();

		AddComponent<ECS::CTransform>(entity);

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
#include <fypch.hpp>
#include "Scene.hpp"

namespace Frosty
{

	Scene::Scene()
	{

	}

	void Scene::Init()
	{
		m_EntityManager.reset(FY_NEW ECS::EntityManager());
	}

	void Scene::OnInput()
	{

	}

	void Scene::OnUpdate()
	{

	}

	void Scene::Render()
	{

	}

	const std::shared_ptr<ECS::Entity>& Scene::At(size_t index) const
	{
		return m_EntityManager->At(index);
	}

	std::shared_ptr<ECS::Entity>& Scene::CreateEntity()
	{
		return m_EntityManager->Create();
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

		// Remove from system here

		m_EntityManager->Remove(entity);
	}
}
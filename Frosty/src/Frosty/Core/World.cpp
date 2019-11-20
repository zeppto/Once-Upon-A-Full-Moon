#include <fypch.hpp>
#include "World.hpp"
#include"Frosty/API/AssetManager/AssetManager.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	void World::Init()
	{
		//AssetManager::Init();
		AssetManager::Get()->LoadFiles();
	}

	void World::Awake()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->Init();
		}

		CreateScene();
	}

	void World::OnStart()
	{
		if (!m_DestroyedEntities.empty()) HandleDestroyedEntities();
		if (m_DestroyRoom != -1) HandleDestroyedRoom();

		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->OnStart();
		}
	}

	void World::OnInput()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->OnInput();
		}
	}

	void World::OnUpdate()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->OnUpdate();
		}
	}

	void World::OnEvent(BaseEvent& e)
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->OnEvent(e);
		}
	}

	void World::BeginScene()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->BeginScene();
		}
	}

	void World::Render()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->Render();
		}
	}

	void World::UpdateSystems(const std::shared_ptr<ECS::Entity>& entity)
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->UpdateEntityComponent(entity);
		}
	}

	std::unique_ptr<Scene>& World::CreateScene()
	{
		m_Scene.reset(FY_NEW Scene());
		m_Scene->Init();

		auto& app = Application::Get();
		auto& win = app.GetWindow();

		// Creates a camera every time a scene is initiated
		auto& camEntity = CreateEntity({ 0.0f, 40.0f, 25.0f }, { 0.0f, 0.0f, 0.0f });
		AddComponent<ECS::CCamera>(camEntity, 60.0f, (float)(win.GetViewport().z / win.GetViewport().w), 0.01f, 1000.0f);
		m_Scene->AddCamera(camEntity);

		return m_Scene;
	}

	void World::DestroyScene()
	{
		m_Scene.reset();
	}

	void World::SetSceneCamera(const std::shared_ptr<ECS::Entity>& entity)
	{
		m_Scene->AddCamera(entity);
	}

	void World::DestroyGroup(int32_t groupId)
	{
		FY_CORE_ASSERT(groupId == 0 || groupId == 1, "That groupId doesn't exist!");

		m_DestroyRoom = groupId;
	}

	void World::HandleDestroyedRoom()
	{
		auto& entityGroup = m_Scene->GetEntityManager()->GetEntityGroup(m_DestroyRoom);
		size_t groupSize = entityGroup.size();
		for (size_t i = groupSize; i-- > 0; i)
		{
			RemoveEntity(entityGroup[i]);
		}

		m_DestroyRoom = -1;
	}

	size_t World::GetCurrentRoom() const
	{
		return m_CurrentRoom;
	}

	void World::ChangeCurrentRoom()
	{
		m_CurrentRoom = (m_CurrentRoom + 1) % 2;
	}

	std::shared_ptr<ECS::Entity>& World::CreateEntity(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool isStatic)
	{
		return m_Scene->CreateEntity(position, rotation, scale, isStatic);
	}

	void World::RemoveEntity(const std::shared_ptr<ECS::Entity>& entity)
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->RemoveEntity(entity);
		}

		m_Scene->RemoveEntity(entity);
	}

	void World::AddToDestroyList(const std::shared_ptr<ECS::Entity>& entity)
	{
		m_DestroyedEntities.emplace_back(entity);
	}

	void World::AddToGroup(const std::shared_ptr<ECS::Entity>& entity, bool current)
	{
		if (current) m_Scene->GetEntityManager()->AddToGroup(m_CurrentRoom, entity);
		else m_Scene->GetEntityManager()->AddToGroup((m_CurrentRoom + 1) % 2, entity);
	}

	void World::PrintWorld()
	{
		// Scene (Entities & Components)
		m_Scene->PrintScene();

		// Systems
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			if (i == m_TotalSystems - 1) FY_CORE_INFO("\n{0}\t\t==============DONE PRINT WORLD INFO==============\n\n\n", *m_Systems[i]);
			else FY_CORE_INFO("\n{0}", *m_Systems[i]);
		}
	}

	void World::HandleDestroyedEntities()
	{
		size_t entitySize = m_DestroyedEntities.size() - 1;
		for (size_t i = entitySize; i-- > 0; i)
		{
			RemoveEntity(m_DestroyedEntities[i]);
			m_DestroyedEntities.erase(m_DestroyedEntities.begin() + i);
		}
	}
}

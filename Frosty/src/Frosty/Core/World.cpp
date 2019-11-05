#include <fypch.hpp>
#include "World.hpp"
//#include "Frosty/API/AssetManager.hpp"
#include"Frosty/API/AssetManager/AM.hpp"
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
		HandleDestroyedEntities();

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

	std::unique_ptr<Scene>& World::CreateScene()
	{
		m_Scene.reset(FY_NEW Scene());
		m_Scene->Init();

		auto& app = Application::Get();
		auto& win = app.GetWindow();

		// Creates a camera every time a scene is initiated
		auto& camEntity = CreateEntity();
		auto& camTransform = GetComponent<ECS::CTransform>(camEntity);
		camTransform.Position = glm::vec3(0.0f, 40.0f, 25.0f);
		camTransform.Rotation = glm::vec3(-90.0f, -65.0f, 0.0f);
		AddComponent<ECS::CCamera>(camEntity, 60.0f, (float)(win.GetViewport().z / win.GetViewport().w), 0.01f, 1000.0f);
		m_Scene->AddCamera(camEntity);
		//AddComponent<ECS::CLight>(camEntity);

		// Temporary
		//auto& obj = CreateEntity();
		//auto& objTransform = GetComponent<ECS::CTransform>(obj);
		//objTransform.Position.x = -10.0f;
		//AddComponent<ECS::CMesh>(obj, AssetManager::GetMesh("3D"));
		//AddComponent<ECS::CMaterial>(obj, AssetManager::GetShader("FlatColor"));
		//AddComponent<ECS::CMotion>(obj, 3.0f);
		//AddComponent<ECS::CController>(obj);
		//
		//auto& light = CreateEntity();
		//auto& lightTransform = GetComponent<ECS::CTransform>(light);
		//lightTransform.Position.z = 10.0f;
		//lightTransform.Rotation.x = 180.0;
		//AddComponent<ECS::CMesh>(light, AssetManager::GetMesh("Cube"));
		//AddComponent<ECS::CMaterial>(light, AssetManager::GetShader("FlatColor"));
		//AddComponent<ECS::CLight>(light);

		return m_Scene;
	}

	void World::DestroyScene()
	{
		m_Scene.reset();
	}

	void World::RemoveEntity(std::shared_ptr<ECS::Entity>& entity)
	{
		m_Scene->RemoveEntity(entity);

		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->RemoveEntity(entity);
		}
	}

	void World::AddToDestroyList(const std::shared_ptr<ECS::Entity>& entity)
	{
		m_DestroyedEntities.emplace_back(entity);
	}

	void World::HandleDestroyedEntities()
	{
		for (size_t i = 0; i < m_DestroyedEntities.size(); i++)
		{
			RemoveEntity(m_DestroyedEntities[i]);
			m_DestroyedEntities.erase(m_DestroyedEntities.begin() + i);
		}
	}
}

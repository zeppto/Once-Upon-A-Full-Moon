#include <fypch.hpp>
#include "World.hpp"
#include "Frosty/API/AssetManager.hpp"
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	void World::Init()
	{
		AssetManager::Init();
	}

	void World::Start()
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->Init();
		}

		CreateScene();

		// TEMPORARY SO I DON'T HAVE TO CREATE AN ENTITY, A MESH AND A MATERIAL ON EVERY RUN //
		//auto& floor = CreateEntity();
		//auto& floorTrans = GetComponent<ECS::CTransform>(floor);
		////floorTransform.Position = glm::vec3();
		//floorTrans.Scale = glm::vec3(100.0f, 0.0f, 100.0f);
		//AddComponent<ECS::CMesh>(floor, AssetManager::GetMesh("Plane"));
		//auto& floorMat = AddComponent<ECS::CMaterial>(floor, AssetManager::GetShader("FlatColor"));
		//floorMat.Albedo = glm::vec4(0.2f, 0.8f, 0.3f, 1.0f);
		//
		//
		//auto& player = CreateEntity();
		//auto& playerTrans = GetComponent<ECS::CTransform>(player);
		//playerTrans.Position.y = 1.0f;
		//AddComponent<ECS::CMesh>(player, AssetManager::GetMesh("Cylinder"));
		//auto& playerMat = AddComponent<ECS::CMaterial>(player, AssetManager::GetShader("FlatColor"));
		//playerMat.Albedo = glm::vec4(1.0f, 0.35f, 0.35f, 1.0f);
		//AddComponent<ECS::CMotion>(player, 10.0f);
		//AddComponent<ECS::CController>(player);
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

	void World::BeginScene(bool editorCamera)
	{
		for (size_t i = 1; i < m_TotalSystems; i++)
		{
			m_Systems[i]->BeginScene(editorCamera);
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
		camTransform.Position = glm::vec3(0.0f, 20.0f, 20.0f);
		camTransform.Rotation = glm::vec3(-90.0f, -50.0f, 0.0f);
		AddComponent<ECS::CCamera>(camEntity, 60.0f, (float)(win.GetViewport().z / win.GetViewport().w), 0.01f, 1000.0f);

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
}
		
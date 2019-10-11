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
		camTransform.Rotation.x = 90.0f;
		AddComponent<ECS::CCamera>(camEntity, camTransform.Position, 60.0f, (float)(win.GetViewport().z / win.GetViewport().w), 0.01f, 1000.0f);

		// TEMPORARY SO I DON'T HAVE TO CREATE AN ENTITY, A MESH AND A MATERIAL ON EVERY RUN //
		auto& objEntity = CreateEntity();
		auto& objTransform = GetComponent<ECS::CTransform>(objEntity);
		objTransform.Position.z = 1.0f;
		AddComponent<ECS::CMesh>(objEntity, AssetManager::GetMesh("Table"));
		AddComponent<ECS::CMaterial>(objEntity, AssetManager::GetShader("FlatColor"));

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

	void World::CreateSystems()
	{

		// Render System //  
		//ECS::RenderSystem* graphicsSystem(new ECS::RenderSystem());
		//std::unique_ptr<ECS::RenderSystem> graphicsSystemPtr{ graphicsSystem };
		//m_Systems[m_TotalSystems++] = std::move(graphicsSystemPtr);
		//
		//// Camera System //  (Always have this before RenderSystem)
		//ECS::CameraSystem* cameraSystem(new ECS::CameraSystem());
		//std::unique_ptr<ECS::CameraSystem> cameraSystemPtr{ cameraSystem };
		//m_Systems[m_TotalSystems++] = std::move(cameraSystemPtr);
	}
}
		
#include <fypch.hpp>
#include "Scene.hpp"

#include "Frosty/Core/Application.hpp"
#include "Frosty/RenderEngine/Renderer.hpp"


namespace Frosty
{
	Scene::Scene()
	{

	}

	void Scene::Init()
	{
		m_EntityManager.reset(FY_NEW ECS::EntityManager());
		ECS::BaseComponentManager::Init();

		auto& win = Application::Get().GetWindow();

		auto& entity = CreateEntity();
		AddComponent<ECS::CCamera>(entity, 60.0f, (float)(win.GetWidth() / win.GetHeight()), 0.01f, 1000.0f);
	}

	void Scene::OnInput()
	{

	}

	void Scene::OnUpdate()
	{

	}

	void Scene::Render()
	{
		auto camManager = GetComponentManager<ECS::CCamera>();
		if (camManager == nullptr)
		{
			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			RenderCommand::Clear();
			return;
		}
		else
		{
			RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
			RenderCommand::Clear();
		}

		Renderer::BeginScene(camManager->GetAll().at(1).ViewProjectionMatrix);
		// Call systems draw function instead of this messy crap

		auto transManager = GetComponentManager<ECS::CTransform>();
		if (transManager == nullptr)
		{
			return;
		}
		auto matManager = GetComponentManager<ECS::CMaterial>();
		if (matManager == nullptr)
		{
			return;
		}
		auto meshManager = GetComponentManager<ECS::CMesh>();
		if (meshManager == nullptr)
		{
			return;
		}
		size_t tot = glm::min(meshManager->Total, matManager->Total);
		for (size_t i = 1; i < tot; i++)
		{
			if (meshManager->GetAll().at(i).Mesh != nullptr)
			{
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), transManager->GetAll().at(2).Position);
				transform = glm::rotate(transform, glm::radians(transManager->GetAll().at(2).Rotation.x), { 1.0f, 0.0f, 0.0f });
				transform = glm::rotate(transform, glm::radians(transManager->GetAll().at(2).Rotation.y), { 0.0f, 1.0f, 0.0f });
				transform = glm::rotate(transform, glm::radians(transManager->GetAll().at(2).Rotation.z), { 0.0f, 0.0f, 1.0f });
				transform = glm::scale(transform, transManager->GetAll().at(2).Scale);


				matManager->GetAll().at(i).UseShader->Bind();
				matManager->GetAll().at(i).UseShader->UploadUniformFloat4("u_Color", matManager->GetAll().at(i).Albedo);
				matManager->GetAll().at(i).UseShader->UploadUniformMat4("u_Transform", transform);

				Renderer::Submit(matManager->GetAll().at(i).UseShader, meshManager->GetAll().at(i).Mesh);
			}
		}
	}

	std::shared_ptr<ECS::Entity>& Scene::At(size_t index)
	{
		return m_EntityManager->At(index);
	}

	const std::shared_ptr<ECS::Entity>& Scene::At(size_t index) const
	{
		return m_EntityManager->At(index);
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

		// Remove from system here

		m_EntityManager->Remove(entity);
	}
}
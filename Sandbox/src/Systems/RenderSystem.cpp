#include <mcspch.hpp>
#include "RenderSystem.hpp"


namespace MCS
{
	void RenderSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMesh>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CMaterial>(), true);
	}

	void RenderSystem::OnUpdate()
	{
	}

	void RenderSystem::Render()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			// Render
			// So the question is, calculate it every frame or save the result in the component
			// and update it only when Position/Rotation/Scale changes
			// Takes up performance if calculated every frame, but the other way will mean
			// 4 * 4 floats which is 4 * 4 * 4 = 64 bytes for EVERY entity
			// For now we calculate it every frame
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
			transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
			transform = glm::scale(transform, m_Transform[i]->Scale);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind(0);
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);
			if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind(0);

			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind(0);
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendMapTexture) m_Materials[i]->BlendMapTexture->Bind(3);
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture1) m_Materials[i]->BlendTexture1->Bind(4);
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture2) m_Materials[i]->BlendTexture2->Bind(5);

			Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);

			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();

			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendMapTexture) m_Materials[i]->BlendMapTexture->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture1) m_Materials[i]->BlendTexture1->Unbind();
			if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture2) m_Materials[i]->BlendTexture2->Unbind();
		}
	}

	void RenderSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Meshes[p_Total] = &world->GetComponent<Frosty::ECS::CMesh>(entity);
			m_Materials[p_Total] = &world->GetComponent<Frosty::ECS::CMaterial>(entity);

			p_Total++;
		}
	}

	void RenderSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

		if (tempIndex > 0)
		{
			p_Total--;
			m_Transform[p_Total] = nullptr;
			m_Meshes[p_Total] = nullptr;
			m_Materials[p_Total] = nullptr;

			if (p_Total > 1)
			{
				//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

				if (p_Total > tempIndex)
				{
					std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
					p_EntityMap[entityToUpdate] = tempIndex;
				}
			}

			p_EntityMap.erase(entity);
		}
	}
}
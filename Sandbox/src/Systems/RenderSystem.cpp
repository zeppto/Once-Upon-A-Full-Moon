#include <mcspch.hpp>
#include "RenderSystem.hpp"


namespace MCS
{
	const std::string RenderSystem::NAME = "Render";

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
			if (m_Meshes[i]->RenderMesh)
			{
				if (!m_Transform[i]->IsStatic)
				{
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
					transform = glm::scale(transform, m_Transform[i]->Scale);
					m_Transform[i]->ModelMatrix = transform;
				}

				if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind(0);
				if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
				if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);

				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind(0);
				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);

				if (m_Materials[i]->UseShader->GetName() == "UI" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind(0);

				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->DiffuseTexture)m_Materials[i]->DiffuseTexture->Bind(0);
				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(1);
				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Bind(2);
				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendMapTexture) m_Materials[i]->BlendMapTexture->Bind(3);
				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture1) m_Materials[i]->BlendTexture1->Bind(4);
				if (m_Materials[i]->UseShader->GetName() == "BlendShader" && m_Materials[i]->BlendTexture2) m_Materials[i]->BlendTexture2->Bind(5);

				if (m_Materials[i]->UseShader->GetName() == "Animation")
				{
					Frosty::Renderer::AnimSubmit(m_Materials[i], m_Meshes[i]->Mesh, m_Transform[i]->ModelMatrix);
				}
				else
				{
					//Frosty::Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, m_Transform[i]->ModelMatrix);
				}

				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Unbind();
				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Unbind();
				if (m_Materials[i]->UseShader->GetName() == "Animation" && m_Materials[i]->SpecularTexture) m_Materials[i]->SpecularTexture->Unbind();

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

			Frosty::Renderer::AddToRenderer(m_Materials.at(p_Total), m_Meshes.at(p_Total)->Mesh, m_Transform.at(p_Total));

			p_Total++;

		}
	}

	void RenderSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;

			Frosty::Renderer::RemoveFromRenderer(m_Materials[it->second]->EntityPtr->Id,m_Meshes[it->second]->Mesh->GetName(),m_Transform[it->second]->EntityPtr->Id);

			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Meshes[p_Total] = nullptr;
			m_Materials[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}


			p_EntityMap.erase(entity);
		}
	}

	void RenderSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CMesh* meshPtr = world->GetComponentAddress<Frosty::ECS::CMesh>(entity);
			Frosty::ECS::CMaterial* materialPtr = world->GetComponentAddress<Frosty::ECS::CMaterial>(entity);

			m_Transform[it->second] = transformPtr;
			m_Meshes[it->second] = meshPtr;
			m_Materials[it->second] = materialPtr;

			Frosty::Renderer::UpdateEntity(m_Materials[it->second]->EntityPtr->Id, m_Materials.at(it->second), m_Meshes[it->second]->Mesh->GetName(), m_Meshes.at(it->second)->Mesh, m_Transform[it->second]->EntityPtr->Id, m_Transform.at(it->second));

		}
	}

	std::string RenderSystem::GetInfo() const
	{
		std::stringstream retInfo;
		retInfo << "\t-----------" << NAME << " System Info-----------\n";
		retInfo << "\t\t---------Entity Map---------\n";
		retInfo << "\t\tEntity Id\tEntity Address\t\tEntity Refs\tArray Index\n";
		for (auto& em : p_EntityMap)
		{
			retInfo << "\t\t" << em.first->Id << "\t\t" << em.first << "\t\t" << em.first.use_count() << "\t" << em.second << "\n";
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t\t------Component Array(s)------\n";
		retInfo << "\n\t\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
		for (size_t i = 1; i < p_Total; i++)
		{
			retInfo << "\t\t" << i << "\t" << m_Transform[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_Transform[i]->EntityPtr << "\t\t" << m_Transform[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Meshes[i] << "\t" << m_Meshes[i]->EntityPtr->Id << "\t\t" << m_Meshes[i]->EntityPtr << "\t\t" << m_Meshes[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Materials[i] << "\t" << m_Materials[i]->EntityPtr->Id << "\t\t" << m_Materials[i]->EntityPtr << "\t\t" << m_Materials[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}
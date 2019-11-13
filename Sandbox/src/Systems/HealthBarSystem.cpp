#include <mcspch.hpp>
#include <algorithm>
#include "HealthBarSystem.hpp"

namespace MCS
{

	const std::string HealthBarSystem::NAME = "Health Bar";

	void HealthBarSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealthBar>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
	}

	void HealthBarSystem::OnUpdate()
	{
	}

	void HealthBarSystem::Render()
	{
		Frosty::Renderer::GameCameraProps cam = Frosty::Renderer::GetCamera();
		glm::vec3 ndcSpacePos;
		for (size_t i = 1; i < p_Total; i++)
		{
			if (true)
			{
				float pivot = 0.5f;

				glm::vec3 TmaxHP = glm::vec3(m_Health[i]->MaxHealth, 1, 1);
				glm::vec3 TcurrHP = glm::vec3(m_Health[i]->CurrentHealth, 1, 1);

				//translate
				//world position to screen position
				glm::vec4 clipSpace = cam.ProjectionMatrix * (cam.ViewMatrix * glm::vec4(m_Transform[i]->Position + m_HealthBar[i]->BarOffset + glm::vec3(TcurrHP.x / TmaxHP.x - 1.0f, 0.0f, 0.0f), 1.0f));
				ndcSpacePos = glm::vec3(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w);

				m_HealthBar[i]->hpTransform = glm::translate(glm::mat4{ 1.0f }, ndcSpacePos);


				//scale
				//scale calc
				float camDistance = glm::distance(Frosty::Renderer::GetCamera().CameraPosition, m_Transform[i]->Position + m_HealthBar[i]->BarOffset);

				m_HealthBar[i]->hpTransform = glm::scale(m_HealthBar[i]->hpTransform, (glm::vec3(((TcurrHP.x / TmaxHP.x) * 2), 1.0f, 1.0f) * (1 / camDistance)));


				//rotate
				m_HealthBar[i]->hpTransform = glm::rotate(m_HealthBar[i]->hpTransform, glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });

			}

			if (false)
			{
				int TmaxHP = m_Health[i]->MaxHealth;
				int TcurrHP = m_Health[i]->CurrentHealth;

				float scaleFact = 1.0f;

				float temp = std::max((TcurrHP / TmaxHP) - 1, 0) * scaleFact;

				glm::vec4 clipSpace = cam.ProjectionMatrix * (cam.ViewMatrix * glm::vec4(m_Transform[i]->Position + m_HealthBar[i]->BarOffset + glm::vec3(temp, 0.0f, 0.0f), 1.0f));
				
				ndcSpacePos = glm::vec3(clipSpace.x / clipSpace.w, clipSpace.y / clipSpace.w, clipSpace.z / clipSpace.w);

				m_HealthBar[i]->hpTransform = glm::translate(glm::mat4{ 1.0f }, ndcSpacePos);

				//scale
				//scale calc
				float camDistance = glm::distance(Frosty::Renderer::GetCamera().CameraPosition,		m_Transform[i]->Position + m_HealthBar[i]->BarOffset);
				m_HealthBar[i]->hpTransform = glm::scale(m_HealthBar[i]->hpTransform, glm::vec3(std::max((TcurrHP / TmaxHP), 0), 1.0f, 1.0f) * (1 / camDistance));


				//rotate
				m_HealthBar[i]->hpTransform = glm::rotate(m_HealthBar[i]->hpTransform, glm::radians(90.0f), { 1.0f, 0.0f, 0.0f });

			}

			if (m_HealthBar[i]->Texture && m_HealthBar[i]->UseShader->GetName() == "UI") m_HealthBar[i]->Texture->Bind(0);			

			Frosty::Renderer::Submit2d(m_HealthBar[i]->Texture.get(), m_HealthBar[i]->UseShader, m_HealthBar[i]->Mesh, m_HealthBar[i]->hpTransform);

			if (m_HealthBar[i]->UseShader->GetName() == "UI" && m_HealthBar[i]->Texture) m_HealthBar[i]->Texture->Unbind();
		}
	}

	void HealthBarSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Health[p_Total] = &world->GetComponent<Frosty::ECS::CHealth>(entity);
			m_HealthBar[p_Total] = &world->GetComponent<Frosty::ECS::CHealthBar>(entity);

			if (!m_HealthBar[p_Total]->Mesh)
			{
				m_HealthBar[p_Total]->Mesh = Frosty::AssetManager::GetMesh("UIPlane");
				m_HealthBar[p_Total]->UseShader = Frosty::AssetManager::GetShader("UI");
				m_HealthBar[p_Total]->Texture = Frosty::AssetManager::GetTexture2D("red");
			}

			
			p_Total++;
		}
	}

	void HealthBarSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;
			m_HealthBar[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void HealthBarSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CHealth* healthPtr = world->GetComponentAddress<Frosty::ECS::CHealth>(entity);
			Frosty::ECS::CHealthBar* healthBarPtr = world->GetComponentAddress<Frosty::ECS::CHealthBar>(entity);

			m_Transform[it->second] = transformPtr;
			m_Health[it->second] = healthPtr;
			m_HealthBar[it->second] = healthBarPtr;
		}
	}

	std::string HealthBarSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Health[i] << "\t" << m_Health[i]->EntityPtr->Id << "\t\t" << m_Health[i]->EntityPtr << "\t\t" << m_Health[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_HealthBar[i] << "\t" << m_HealthBar[i]->EntityPtr->Id << "\t\t" << m_HealthBar[i]->EntityPtr << "\t\t" << m_HealthBar[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

}
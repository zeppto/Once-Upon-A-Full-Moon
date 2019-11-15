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
		for (size_t i = 1; i < p_Total; i++)
		{

			float TmaxHP = m_Health[i]->MaxHealth;
			float TcurrHP = m_Health[i]->CurrentHealth;

			float sizeFact = 0.05 * TmaxHP;

			float translateFact = (std::clamp((TcurrHP / TmaxHP), 0.0f, 1.0f) - 1) * (1.0f + sizeFact);

			float scaleFact = std::clamp((TcurrHP / TmaxHP), 0.0f, 1.0f) * (1.0f + sizeFact);

			m_HealthBar[i]->Translate = glm::vec3(m_Transform[i]->Position.x + translateFact, m_Transform[i]->Position.y, m_Transform[i]->Position.z);

			m_HealthBar[i]->Scale = glm::vec3(scaleFact, (0.3f + 0.05 * sizeFact), 1.0f);


			if (m_HealthBar[i]->Texture && m_HealthBar[i]->UseShader->GetName() == "HealthBar") m_HealthBar[i]->Texture->Bind(0);			

			Frosty::Renderer::SubmitHealthBar(m_HealthBar[i]->UseShader, m_HealthBar[i]->Mesh, m_HealthBar[i]->Translate, m_HealthBar[i]->Scale, m_HealthBar[i]->BarOffset);

			if (m_HealthBar[i]->UseShader->GetName() == "HealthBar" && m_HealthBar[i]->Texture) m_HealthBar[i]->Texture->Unbind();

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
				m_HealthBar[p_Total]->UseShader = Frosty::AssetManager::GetShader("HealthBar");
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
			p_Total--;

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
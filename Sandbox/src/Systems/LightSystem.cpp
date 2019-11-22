#include <mcspch.hpp>
#include "LightSystem.hpp"


namespace MCS
{
	const std::string LightSystem::NAME = "Light";

	void LightSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLight>(), true);
	}

	void LightSystem::OnUpdate()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Light[i]->Origin != nullptr)
			{
				m_Transform[i]->Position = m_Light[i]->Origin->Position + m_Light[i]->Offset;
			}
		}
	}

	void LightSystem::Render()
	{
		auto& win = Frosty::Application::Get().GetWindow();

		// Optimization: Could send in to Renderer how many point and directional lights we have to reserve that space in vectors.
		for (size_t i = 1; i < p_Total; i++)
		{
			/*if (m_Light[i]->Type == Frosty::ECS::CLight::LightType::Point)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Position, m_Light[i]->Strength, m_Light[i]->Radius);
			}
			else if (m_Light[i]->Type == Frosty::ECS::CLight::LightType::Directional)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Rotation, m_Light[i]->Strength);
			}*/
		}
	}

	void LightSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Light[p_Total] = &world->GetComponent<Frosty::ECS::CLight>(entity);

			
			Frosty::Renderer::AddLight(m_Light[p_Total], m_Transform[p_Total]);


			/*if (m_Light[p_Total]->Type == Frosty::ECS::CLight::LightType::Point)
			{
				Frosty::Renderer::AddLight(m_Light[p_Total]->Color, m_Transform[p_Total]->Position, m_Light[p_Total]->Strength, m_Light[p_Total]->Radius);
			}
			else if (m_Light[p_Total]->Type == Frosty::ECS::CLight::LightType::Directional)
			{
				Frosty::Renderer::AddLight(m_Light[p_Total]->Color, m_Transform[p_Total]->Rotation, m_Light[p_Total]->Strength);
			}*/

			p_Total++;
		}
	}

	void LightSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			
			//UpdateEntityComponent(entity);
			Frosty::Renderer::RemoveLight(entity);

			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Light[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void LightSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CLight* lightPtr = world->GetComponentAddress<Frosty::ECS::CLight>(entity);

			m_Transform[it->second] = transformPtr;
			m_Light[it->second] = lightPtr;

			Frosty::Renderer::UppdateLight(m_Light[it->second], m_Transform[it->second]);

		}
	}

	std::string LightSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Light[i] << "\t" << m_Light[i]->EntityPtr->Id << "\t\t" << m_Light[i]->EntityPtr << "\t\t" << m_Light[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}
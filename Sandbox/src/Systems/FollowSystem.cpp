#include <mcspch.hpp>
#include "FollowSystem.hpp"

namespace MCS
{
	const std::string FollowSystem::NAME = "Follow";

	void FollowSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CFollow>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CPhysics>(), true);
	}

	void FollowSystem::OnUpdate()
	{
		//for (size_t i = 1; i < p_Total; i++)
		//{
		//	if (m_Follow[i]->Target)
		//	{
		//		float distance = glm::distance({ m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z }, m_Follow[i]->Target->Position);
		//		if (distance > m_Follow[i]->StopDistance)
		//		{
		//			m_Physics[i]->Velocity = glm::normalize(m_Follow[i]->Target->Position - glm::vec3(m_Transform[i]->Position.x, 0.0f, m_Transform[i]->Position.z)) * m_Physics[i]->Speed;
		//		}
		//		else
		//		{
		//			m_Physics[i]->Velocity = glm::vec3(0.0f);
		//		}
		//	}
		//}
	}

	void FollowSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_Follow[p_Total] = &world->GetComponent<Frosty::ECS::CFollow>(entity);
			m_Physics[p_Total] = &world->GetComponent<Frosty::ECS::CPhysics>(entity);

			p_Total++;
		}
	}

	void FollowSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Follow[p_Total] = nullptr;
			m_Physics[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void FollowSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CFollow* followPtr = world->GetComponentAddress<Frosty::ECS::CFollow>(entity);
			Frosty::ECS::CPhysics* physicsPtr = world->GetComponentAddress<Frosty::ECS::CPhysics>(entity);

			m_Transform[it->second] = transformPtr;
			m_Follow[it->second] = followPtr;
			m_Physics[it->second] = physicsPtr;
		}
	}

	std::string FollowSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_Follow[i] << "\t" << m_Follow[i]->EntityPtr->Id << "\t\t" << m_Follow[i]->EntityPtr << "\t\t" << m_Follow[i]->EntityPtr.use_count() << "\n";
			retInfo << "\t\t" << i << "\t" << m_Physics[i] << "\t" << m_Physics[i]->EntityPtr->Id << "\t\t" << m_Physics[i]->EntityPtr << "\t\t" << m_Physics[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}
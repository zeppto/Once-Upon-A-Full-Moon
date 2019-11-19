#include <mcspch.hpp>
#include "ChestSystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	const std::string ChestSystem::NAME = "Chest";

	void ChestSystem::Init()
	{
		m_World = Frosty::Application::Get().GetWorld().get();
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CChest>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CHealth>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);

		std::srand((unsigned)std::time(0));
	}

	void ChestSystem::OnUpdate()
	{
		m_CoolDown += Frosty::Time::DeltaTime();

		/*for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Chest[i]->EntityPtr != nullptr && m_Health[i]->CurrentHealth <= 0)
			{
				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Chest[i]->EntityPtr);
			}
		}*/

		/*for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Chest[i]->EntityPtr != nullptr && Frosty::InputManager::IsKeyPressed(FY_KEY_G) && m_CoolDown > 1.0f)
			{
				//m_World->AddComponent<Frosty::ECS::CDestroy>(m_Chest[i]->EntityPtr);
				//SpawnItem(i);


				m_Health[i]->CurrentHealth--;

				m_CoolDown = 0.0f;
				FY_INFO("{0}", m_Health[i]->CurrentHealth);
			}
		}*/

		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Chest[i]->EntityPtr != nullptr && Frosty::InputManager::IsKeyPressed(FY_KEY_G) && m_CoolDown > 1.0f)
			{
				m_World->AddComponent<Frosty::ECS::CDestroy>(m_Chest[i]->EntityPtr);
				SpawnItem(i);
				m_CoolDown = 0.0f;

				//FY_INFO("{0}", m_Health[i]->CurrentHealth);
			}
		}
	}

	void ChestSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			m_Chest[p_Total] = &m_World->GetComponent<Frosty::ECS::CChest>(entity);
			m_Health[p_Total] = &m_World->GetComponent<Frosty::ECS::CHealth>(entity);
			m_Transform[p_Total] = &m_World->GetComponent<Frosty::ECS::CTransform>(entity);
			p_Total++;
		}
	}

	void ChestSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_Chest[p_Total] = nullptr;
			m_Health[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void ChestSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CChest* chestPtr = world->GetComponentAddress<Frosty::ECS::CChest>(entity);
			Frosty::ECS::CHealth* healthPtr = world->GetComponentAddress<Frosty::ECS::CHealth>(entity);
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);

			m_Transform[it->second] = transformPtr;
			m_Health[it->second] = healthPtr;
			m_Chest[it->second] = chestPtr;
		}
	}

	std::string ChestSystem::GetInfo() const
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
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void ChestSystem::SpawnItem(size_t index)
	{
		m_RandItem = (rand() % 6) + 1;

		auto& item = m_World->CreateEntity({ m_Transform.at(index)->Position }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f });
		m_World->AddComponent<Frosty::ECS::CMesh>(item, Frosty::AssetManager::GetMesh("pCube1"));
		m_World->AddComponent<Frosty::ECS::CMaterial>(item, Frosty::AssetManager::GetShader("FlatColor"));
		m_World->AddComponent<Frosty::ECS::CPhysics>(item, Frosty::AssetManager::GetBoundingBox("pCube1"), 6.0f);

		

		switch (m_RandItem)
		{
		case 1:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::HealingPotion);
			break;
		case 2:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::IncHealthPotion);
			break;
		case 3:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::SpeedPotion);
			break;
		case 4:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::SpeedBoot);
			break;
		case 5:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::Sword);
			break;
		case 6:
			m_World->AddComponent<Frosty::ECS::CLootable>(item, Frosty::ECS::CLootable::LootType::Arrow);
			break;
		default:
			break;
		}
	}
}

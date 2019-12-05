#include <mcspch.hpp>
#include "WitchCircleSystem.hpp"
#include "Frosty/Events/AbilityEvent.hpp"

namespace MCS
{

	const std::string WitchCircleSystem::NAME = "Witch Circle";

	void WitchCircleSystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CWitchCircle>(), true);

		m_World = Frosty::Application::Get().GetWorld().get();
	}

	void WitchCircleSystem::OnUpdate()
	{
		if (m_CurrentActiveWC != 0)
		{
			if (glm::distance(m_Transform[m_CurrentActiveWC]->Position, m_WitchCircle[m_CurrentActiveWC]->Enchanter->Position) <= 2.f)
			{
				auto& witchCircleHealth = m_World->GetComponent<Frosty::ECS::CHealth>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
				auto& witchCircleHealthBar = m_World->GetComponent<Frosty::ECS::CHealthBar>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);

				if (Frosty::Time::CurrentTime() - m_WitchCircle[m_CurrentActiveWC]->WitchCircleTimer >= 0.05f && witchCircleHealth.CurrentHealth < witchCircleHealth.MaxHealth)
				{
					witchCircleHealth.CurrentHealth += 1;

					if (witchCircleHealthBar.Background == nullptr)
					{
						auto& barBackground = m_World->CreateEntity(m_Transform[m_CurrentActiveWC]->Position, m_Transform[m_CurrentActiveWC]->Rotation, m_Transform[m_CurrentActiveWC]->Scale);
						m_World->AddComponent<Frosty::ECS::CHealth>(barBackground, witchCircleHealth.MaxHealth);
						auto& barBackgroundComp = m_World->AddComponent<Frosty::ECS::CHealthBar>(barBackground, witchCircleHealthBar.BarOffset, Frosty::AssetManager::GetMesh("UIPlane"), Frosty::AssetManager::GetShader("HealthBar"), Frosty::AssetManager::GetTexture2D("black"));
						witchCircleHealthBar.Background = &barBackgroundComp;
					}

					m_WitchCircle[m_CurrentActiveWC]->WitchCircleTimer = Frosty::Time::CurrentTime();
				}
				else if (witchCircleHealth.CurrentHealth >= witchCircleHealth.MaxHealth)
				{
					RemoveHealthBar();

					// Send event to generate an element
					Frosty::EventBus::GetEventBus()->Publish<Frosty::UpgradeWeaponEvent>(Frosty::UpgradeWeaponEvent());
				}
			}
			else
			{
				ResetHealthBar();
			}
		}
	}

	void WitchCircleSystem::OnEvent(Frosty::BaseEvent& e)
	{
		switch (e.GetEventType())
		{
		case Frosty::EventType::ActivateWitchCircle:
			OnActiveWitchCircleEvent(static_cast<Frosty::ActivateWitchCircleEvent&>(e));
			break;
		default:
			break;
		}
	}

	void WitchCircleSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);

			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_WitchCircle[p_Total] = &world->GetComponent<Frosty::ECS::CWitchCircle>(entity);

			p_Total++;
		}
	}

	void WitchCircleSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;

			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_WitchCircle[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void WitchCircleSystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CWitchCircle* witchCirclePtr = world->GetComponentAddress<Frosty::ECS::CWitchCircle>(entity);

			m_Transform[it->second] = transformPtr;
			m_WitchCircle[it->second] = witchCirclePtr;
		}
	}

	std::string WitchCircleSystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_WitchCircle[i] << "\t" << m_WitchCircle[i]->EntityPtr->Id << "\t\t" << m_WitchCircle[i]->EntityPtr << "\t\t" << m_WitchCircle[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}

	void WitchCircleSystem::OnActiveWitchCircleEvent(Frosty::ActivateWitchCircleEvent& e)
	{
		auto& enchanter = e.GetEntity();

		for (size_t i = 1; i < p_Total && m_CurrentActiveWC == 0; i++)
		{
			auto& enchanterTransform = m_World->GetComponent<Frosty::ECS::CTransform>(enchanter);
			
			// If enchanter is within reach --> activate the hexcircle
			if (glm::distance(m_Transform[i]->Position, enchanterTransform.Position) <= 2.f && !m_WitchCircle[i]->Deployed)
			{
				if (m_World->HasComponent<Frosty::ECS::CPlayer>(enchanter))
				{
					auto& playerComp = m_World->GetComponent<Frosty::ECS::CPlayer>(enchanter);
					auto& inventoryComp = m_World->GetComponent<Frosty::ECS::CInventory>(enchanter);

					if (inventoryComp.CurrentWolfsbane > 0)
					{
						auto& weaponComp = m_World->GetComponent<Frosty::ECS::CWeapon>(playerComp.Weapon->EntityPtr);
						if (!weaponComp.IsFullyUpgraded)
						{
							m_CurrentActiveWC = i;
							m_WitchCircle[i]->Enchanter = &enchanterTransform;
						}
						else
							SetHUDText(enchanter, "Weapon Is Fully Upgraded");
					}
					else
						SetHUDText(enchanter, "Not Enough Wolfsbane");
				}
			}
		}
	}

	void WitchCircleSystem::ResetHealthBar()
	{
		auto& witchCircleHealth = m_World->GetComponent<Frosty::ECS::CHealth>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
		auto& witchCircleHealthBar = m_World->GetComponent<Frosty::ECS::CHealthBar>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);

		// Remove healthbar (witchCircle + background)
		if (witchCircleHealthBar.Background->EntityPtr != nullptr)
		{
			auto& backgroundBar = witchCircleHealthBar.Background->EntityPtr;

			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(witchCircleHealthBar.Background->EntityPtr))
			{
				m_World->AddComponent<Frosty::ECS::CDestroy>(witchCircleHealthBar.Background->EntityPtr);
			}
			witchCircleHealthBar.Background = nullptr;
			m_WitchCircle[m_CurrentActiveWC]->Enchanter = nullptr;
			witchCircleHealth.CurrentHealth = 0;
			m_CurrentActiveWC = 0;
		}
	}

	void WitchCircleSystem::RemoveHealthBar()
	{
		// Remove light and particles from witchCircle (nullify)
		auto& lightComp = m_World->GetComponent<Frosty::ECS::CLight>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
		lightComp.Radius = 0.f;
		auto& particleComp = m_World->GetComponent<Frosty::ECS::CParticleSystem>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
		particleComp.MaxParticles = 1;
		particleComp.EmitCount = 0;

		// Cannot remove components in entity due to address ptr failures, just like remove entities	~ W-_-W ~
		//m_World->RemoveComponent<Frosty::ECS::CLight>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
		//m_World->RemoveComponent<Frosty::ECS::CParticleSystem>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);

		auto& witchCircleHealth = m_World->GetComponent<Frosty::ECS::CHealth>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
		auto& witchCircleHealthBar = m_World->GetComponent<Frosty::ECS::CHealthBar>(m_WitchCircle[m_CurrentActiveWC]->EntityPtr);
	
		// Remove healthbar (witchCircle + background)
		if (witchCircleHealthBar.Background->EntityPtr != nullptr)
		{
			auto& backgroundBar = witchCircleHealthBar.Background->EntityPtr;

			if (!m_World->HasComponent<Frosty::ECS::CDestroy>(witchCircleHealthBar.Background->EntityPtr))
			{
				m_World->AddComponent<Frosty::ECS::CDestroy>(witchCircleHealthBar.Background->EntityPtr);
			}

			//witchCircleHealthBar.UseShader = Frosty::AssetManager::GetShader("FlatColor");
			//witchCircleHealthBar.BarOffset = glm::vec3(0.f, -5.f, 0.f);
			m_WitchCircle[m_CurrentActiveWC]->Enchanter = nullptr;
			m_WitchCircle[m_CurrentActiveWC]->Deployed = true;
			witchCircleHealth.CurrentHealth = 0;

			m_CurrentActiveWC = 0;
		}
	}

	void WitchCircleSystem::SetHUDText(const std::shared_ptr<Frosty::ECS::Entity>& entity, std::string text)
	{
		if (m_World->HasComponent<Frosty::ECS::CGUI>(entity) && m_World->HasComponent<Frosty::ECS::CPlayer>(entity))
		{
			auto& playerComp = m_World->GetComponent<Frosty::ECS::CPlayer>(entity);
			auto& HUD = m_World->GetComponent<Frosty::ECS::CGUI>(entity);

			HUD.Layout.texts.at(6).SetText(text);
			playerComp.PickUpTextTimer = Frosty::Time::CurrentTime();
		}
	}
}
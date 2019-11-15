#include "mcspch.hpp"
#include "GUISystem.hpp"
#include "Frosty/API/AssetManager/AssetManager.hpp"

namespace MCS
{
	const std::string GUISystem::NAME = "GUI";

	void GUISystem::Init()
	{
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
		p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CGUI>(), true);
	}

	void GUISystem::OnUpdate()
	{
		//for (size_t i = 1; i < p_Total; i++)
		//{

		//}
	}

	void GUISystem::Render()
	{
		for (size_t i = 1; i < p_Total; i++)
		{
			//Render all text
			for (int j = 0; j < m_GUI[i]->layout.texts.size(); j++)
			{
				Frosty::UIText& currText = m_GUI[i]->layout.texts[j];
				Frosty::Renderer::SubmitText(m_GUI[i]->textShader, currText.GetVertexArray(), currText.GetText(), currText.GetPosition(), currText.GetColor(), currText.GetFontScale());
			}
		}
	}

	void GUISystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
		{
			p_EntityMap.emplace(entity, p_Total);
			auto& world = Frosty::Application::Get().GetWorld();
			m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
			m_GUI[p_Total] = &world->GetComponent<Frosty::ECS::CGUI>(entity);

			m_GUI[p_Total]->textShader = Frosty::AssetManager::GetShader("Text");

			////Debug test

			//Frosty::UIText text(glm::vec2(0.0f), "Hello team");
			//m_GUI[p_Total]->texts.push_back(text);
			//m_GUI[p_Total]->textsCount += 1;
			//////////////

			p_Total++;
		}
	}

	void GUISystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			p_Total--;
			auto& entityToUpdate = m_Transform[p_Total]->EntityPtr;
			m_Transform[p_Total] = nullptr;
			m_GUI[p_Total] = nullptr;

			if (p_Total > it->second)
			{
				p_EntityMap[entityToUpdate] = it->second;
			}

			p_EntityMap.erase(entity);
		}
	}

	void GUISystem::UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
	{
		auto& it = p_EntityMap.find(entity);

		if (it != p_EntityMap.end())
		{
			auto& world = Frosty::Application::Get().GetWorld();
			Frosty::ECS::CTransform* transformPtr = world->GetComponentAddress<Frosty::ECS::CTransform>(entity);
			Frosty::ECS::CGUI* guiPtr = world->GetComponentAddress<Frosty::ECS::CGUI>(entity);

			m_Transform[it->second] = transformPtr;
			m_GUI[it->second] = guiPtr;
		}
	}

	std::string GUISystem::GetInfo() const
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
			retInfo << "\t\t" << i << "\t" << m_GUI[i] << "\t" << m_Transform[i]->EntityPtr->Id << "\t\t" << m_GUI[i]->EntityPtr << "\t\t" << m_GUI[i]->EntityPtr.use_count() << "\n";
			retInfo << "\n"; // Have this last
		}
		retInfo << "\t\t-----------Done-----------\n";
		retInfo << "\t----------------Done----------------\n\n";

		return retInfo.str();
	}
}
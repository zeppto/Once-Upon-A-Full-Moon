#include <mcspch.hpp>
#include "LightSystem.hpp"

void LightSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CLight>(), true);
}

void LightSystem::OnUpdate()
{

}

void LightSystem::Render()
{
	auto& win = Frosty::Application::Get().GetWindow();

	if (p_Total > 1)
	{
		// Optimization: Could send in to Renderer how many point and directional lights we have to reserve that space in vectors.
		for (size_t i = 1; i < p_Total; i++)
		{
			if (m_Light[i]->Type == Frosty::ECS::CLight::Point)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Position, m_Light[i]->Strength, m_Light[i]->Radius);
			}
			else if (m_Light[i]->Type == Frosty::ECS::CLight::Directional)
			{
				Frosty::Renderer::AddLight(m_Light[i]->Color, m_Transform[i]->Rotation, m_Light[i]->Strength);
			}
		}
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

		p_Total++;
	}
}

void LightSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_Light[p_Total] = nullptr;

		//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}
#include "mcspch.hpp"
#include "ParticleSystem.hpp"

void ParticleSystem::Init()
{
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CTransform>(), true);
	p_Signature.set(Frosty::ECS::getComponentTypeID<Frosty::ECS::CParticleSystem>(), true);
}

void ParticleSystem::OnUpdate()
{

}

void ParticleSystem::Render()
{

}

void ParticleSystem::AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	if (Frosty::utils::BitsetFits<Frosty::ECS::MAX_COMPONENTS>(p_Signature, entity->Bitset) && !p_EntityMap.count(entity))
	{
		p_EntityMap.emplace(entity, p_Total);

		auto& world = Frosty::Application::Get().GetWorld();
		m_Transform[p_Total] = &world->GetComponent<Frosty::ECS::CTransform>(entity);
		m_ParticleSystem[p_Total] = &world->GetComponent<Frosty::ECS::CParticleSystem>(entity);

		p_Total++;
	}
}

void ParticleSystem::RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity)
{
	Frosty::ECS::ComponentArrayIndex tempIndex = p_EntityMap[entity];

	if (tempIndex > 0)
	{
		p_Total--;
		m_Transform[p_Total] = nullptr;
		m_ParticleSystem[p_Total] = nullptr;

		if (p_Total > tempIndex)
		{
			std::shared_ptr<Frosty::ECS::Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
			p_EntityMap[entityToUpdate] = tempIndex;
		}

		p_EntityMap.erase(entity);
	}
}

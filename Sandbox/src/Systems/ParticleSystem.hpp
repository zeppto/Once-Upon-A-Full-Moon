#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP
#include "Frosty/API/AssetManager.hpp"

//For now this is a test particle system, the class might as well be called "fire" or whatever this is supposed to be in the end.
//For now, the particle system have a particle system
class ParticleSystem : public Frosty::ECS::BaseSystem
{
public:
	ParticleSystem() = default;
	virtual ~ParticleSystem() = default;

	virtual void Init() override;

	virtual void OnUpdate() override;

	virtual void Render() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CParticleSystem*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_ParticleSystem;
};

#endif // !PARTICLE_SYSTEM_HPP

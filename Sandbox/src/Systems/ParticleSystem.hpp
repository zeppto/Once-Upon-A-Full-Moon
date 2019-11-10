#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

//For now this is a test particle system, the class might as well be called "fire" or whatever this is supposed to be in the end.
//For now, the particle system have a particle system
namespace MCS
{
	class ParticleSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		ParticleSystem() = default;
		virtual ~ParticleSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void Render() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void UpdateParticleSystem(uint32_t systemIndex);
		void EditorUpdateParticleSystem(uint32_t systemIndex);
		void UpdateParticle(uint32_t systemIndex, uint32_t index);
		void ResetParticle(uint32_t systemIndex, uint32_t index);
		void UpdateBuffer(uint32_t systemIndex);

		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CParticleSystem*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_ParticleSystem;
		float m_maxLifetime; //All particles
		bool m_editMode;
	};
}

#endif // !PARTICLE_SYSTEM_HPP
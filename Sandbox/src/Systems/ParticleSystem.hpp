#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

//For now this is a test particle system, the class might as well be called "fire" or whatever this is supposed to be in the end.
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

		void AttachGameCamera(Frosty::ECS::CTransform* gameCamera) { m_CameraTransform = gameCamera; }
		void ChangeParticlesStartColor (Frosty::ECS::CParticleSystem& particleSystem, glm::vec3 color);

	private:
		void UpdateParticleSystem(size_t systemIndex);
		void EditorUpdateParticleSystem(size_t systemIndex);
		void UpdateParticle(size_t systemIndex, size_t index);
		void ResetParticle(size_t systemIndex, size_t index);
		void UpdateGpuData(size_t systemIndex, size_t index, uint32_t particleCount);
		void UpdateGpuData(size_t systemIndex, size_t index);
		void UpdateBuffer(size_t systemIndex);

		uint32_t FindUnusedParticle(size_t systemIndex);
		void SortParticles(size_t systemIndex);

		float Lerp(float a, float b, float f);
		float RandomFloat(float min, float max);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CParticleSystem*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_ParticleSystem;
		bool m_EditMode{ false };

		Frosty::ECS::CTransform* m_CameraTransform{ nullptr };
	};
}

#endif // !PARTICLE_SYSTEM_HPP
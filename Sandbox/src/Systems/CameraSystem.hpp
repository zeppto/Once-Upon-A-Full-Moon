#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

namespace MCS
{
	class CameraSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		CameraSystem() = default;
		virtual ~CameraSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void BeginScene() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

		inline void PrintHello() { FY_FATAL("Hello Camera System!"); }

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CCamera*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Cameras;
	};
}

#endif // !CAMERA_SYSTEM_HPP

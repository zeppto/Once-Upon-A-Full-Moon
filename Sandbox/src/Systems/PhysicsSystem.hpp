#ifndef PHYSICS_SYSTEM_HPP
#define PHYSICS_SYSTEM_HPP

namespace Frosty { class BoolMapLoadedEvent; }
namespace Frosty { class BoolMap; }
namespace Frosty { class UpdateCurrentRoomEvent; }

namespace MCS
{
	class PhysicsSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;
	public:
		PhysicsSystem() = default;
		virtual ~PhysicsSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

		void OnLoadBoolMapEvent(Frosty::BoolMapLoadedEvent& e);
		void OnUpdateCurrentRoomEvent(Frosty::UpdateCurrentRoomEvent& e);

	private:
		void CheckCollision(size_t index);
		void SpawnItem(size_t index);
	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CPhysics*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Physics;

		Frosty::World* m_World = nullptr;

		std::shared_ptr<Frosty::BoolMap> m_Current_BoolMap{nullptr};
		int m_Room_Rotation{0};

		unsigned int m_RandItem = 0;
	};
}
#endif

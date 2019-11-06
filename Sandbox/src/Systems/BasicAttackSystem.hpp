#ifndef BASIC_ATTACK_SYSTEM_HPP
#define BASIC_ATTACK_SYSTEM_HPP

namespace Frosty { class BasicAttackEvent; }

namespace MCS
{
	class BasicAttackSystem : public Frosty::ECS::BaseSystem
	{
	public:
		BasicAttackSystem() = default;
		virtual ~BasicAttackSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	private:
		void OnBasicAttackEvent(Frosty::BasicAttackEvent& e);
		void CreateBoundingBox(size_t index);
		void CreateProjectile(size_t index);

	private:
		std::array<Frosty::ECS::CBasicAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_BasicAttack;

	};
}

#endif // !BASIC_ATTACK_SYSTEM_HPP

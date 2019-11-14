#ifndef ATTACK_SYSTEM_HPP
#define ATTACK_SYSTEM_HPP

namespace Frosty { class SwapWeaponEvent; }

namespace MCS
{
	class AttackSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		AttackSystem() = default;
		virtual ~AttackSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		std::array<Frosty::ECS::CAttack*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Attack;
	};
}
#endif // !WEAPON_SYSTEM_HPP
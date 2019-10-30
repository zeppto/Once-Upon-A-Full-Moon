#ifndef CONSUMABLES_SYSTEM_HPP
#define CONSUMABLES_SYSTEM_HPP
#include "Frosty.h"

class ConsumablesSystem : public Frosty::ECS::BaseSystem
{
public:
	ConsumablesSystem() = default;
	virtual ~ConsumablesSystem() = default;

	virtual void Init() override;

	virtual void OnInput() override;

	virtual void OnUpdate() override;

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CConsumables*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Consumables;
	std::array<Frosty::ECS::CMotion*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Motion;
	std::array<Frosty::ECS::CHealth*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Health;

};
#endif // !CONSUMABLES_SYSTEM_HPP
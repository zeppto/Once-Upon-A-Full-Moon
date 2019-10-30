#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP
#include "Frosty.h"

class RenderSystem : public Frosty::ECS::BaseSystem
{
public:
	RenderSystem() = default;
	virtual ~RenderSystem() = default;

	virtual void Init() override;

	virtual void OnUpdate() override;

	virtual void Render();

	virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

	virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;

private:
	std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
	std::array<Frosty::ECS::CMesh*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Meshes;
	std::array<Frosty::ECS::CMaterial*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Materials;

};

#endif // !RENDER_SYSTEM_HPP

#ifndef WITCHCIRCLE_HPP
#define WITCHCIRCLE_HPP	
#include "Frosty.h"
#include"Frosty/API/AssetManager/AssetManager.hpp"

namespace Frosty { class ActivateWitchCircleEvent; }

namespace MCS
{
	class WitchCircleSystem : public Frosty::ECS::BaseSystem
	{
	public:
		const static std::string NAME;

	public:
		WitchCircleSystem() = default;
		virtual ~WitchCircleSystem() = default;

		virtual void Init() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Frosty::BaseEvent& e) override;

		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
		virtual std::string GetInfo() const override;

	private:
		void OnActiveWitchCircleEvent(Frosty::ActivateWitchCircleEvent& e);
		void ResetHealthBar();
		void RemoveHealthBar();
		void SetHUDText(const std::shared_ptr<Frosty::ECS::Entity>& entity, std::string text);

	private:
		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
		std::array<Frosty::ECS::CWitchCircle*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_WitchCircle;

		Frosty::World* m_World{ nullptr };

		// The index of an active witch circle
		size_t m_CurrentActiveWC = 0;
	};
}
#endif // !WITCHCIRCLE_HPP

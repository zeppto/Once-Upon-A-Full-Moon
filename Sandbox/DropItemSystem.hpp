//#ifndef WITCHCIRCLE_HPP
//#define WITCHCIRCLE_HPP	
//#include "Frosty.h"
//#include"Frosty/API/AssetManager/AssetManager.hpp"
//
//namespace MCS
//{
//	class WitchCircleSystem : public Frosty::ECS::BaseSystem
//	{
//	public:
//		const static std::string NAME;
//
//	public:
//		WitchCircleSystem() = default;
//		virtual ~WitchCircleSystem() = default;
//
//		virtual void Init() override;
//		virtual void OnUpdate() override;
//		virtual void OnEvent(Frosty::BaseEvent& e) override;
//
//		virtual void AddComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//		virtual void RemoveEntity(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//		virtual void UpdateEntityComponent(const std::shared_ptr<Frosty::ECS::Entity>& entity) override;
//		virtual std::string GetInfo() const override;
//
//
//
//	private:
//		std::array<Frosty::ECS::CTransform*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_Transform;
//		std::array<Frosty::ECS::CWitchCircle*, Frosty::ECS::MAX_ENTITIES_PER_COMPONENT> m_WitchCircle;
//
//		Frosty::World* m_World{ nullptr };
//	};
//}
//#endif // !WITCHCIRCLE_HPP

#ifndef SCENE_HPP
#define SCENE_HPP

#include "Frosty/Core/ECS.hpp"
#include "Frosty/RenderEngine/Renderer.hpp"

namespace Frosty
{
	class Scene
	{
	public:
		Scene();
		Scene(const Scene& org) { FY_CORE_ASSERT(false, "Copy constructor in Entity called."); }
		virtual ~Scene() = default;

		// Operators
		Scene& operator=(const Scene& org) { FY_CORE_ASSERT(false, "Assignment operator in Entity called."); return *this; }

		// Scene Functions
		void Init();
		void OnInput();
		void OnUpdate();
		void Render();

		// Entity Functions
		std::shared_ptr<ECS::Entity>& At(size_t index);
		const std::shared_ptr<ECS::Entity>& At(size_t index) const;
		std::shared_ptr<ECS::Entity>& CreateEntity();
		void RemoveEntity(std::shared_ptr<ECS::Entity>& entity);
		inline size_t GetTotalEntities() const { return m_EntityManager->GetTotalEntities(); }
		template<typename ComponentType>
		inline bool HasComponent(const std::shared_ptr<ECS::Entity>& entity)
		{
			return entity->Bitset[ECS::getComponentTypeID<ComponentType>()];
		}

		// Component Functions
		template<typename ComponentType>
		inline ComponentType& GetComponent(const std::shared_ptr<ECS::Entity>& entity)
		{
			static_assert(std::is_base_of<ECS::BaseComponent, ComponentType>::value,
				"ComponentType must inherit from BaseComponent");

			ECS::ComponentID cId = ECS::getComponentTypeID<ComponentType>();

			FY_CORE_ASSERT(entity->Bitset[cId], "Entity({0}) doesn't have this component.", entity->Id);

			return GetComponentManager<ComponentType>()->Get(entity);
		}

		inline ECS::BaseComponent* GetComponentById(size_t id, const std::shared_ptr<ECS::Entity>& entity)
		{
			return m_ComponentManagers[id]->GetTypeComponent(entity);
		}

		template<typename ComponentType, typename... TArgs>
		inline ComponentType& AddComponent(std::shared_ptr<ECS::Entity>& entity, TArgs&&... mArgs)
		{
			static_assert(std::is_base_of<ECS::BaseComponent, ComponentType>::value,
				"ComponentType must inherit from BaseComponent");

			ECS::ComponentID cId = ECS::getComponentTypeID<ComponentType>();

			FY_CORE_ASSERT(!entity->Bitset[cId], "Entity({0}) already has this component.", entity->Id);

			if (!m_ComponentManagers[cId])
			{
				m_ComponentManagers[cId].reset(FY_NEW ECS::ComponentManager<ComponentType>());
			}

			ComponentType& addedComp = GetComponentManager<ComponentType>()->Add(entity, std::forward<TArgs>(mArgs)...);

			// Check which systems needs this component

			return addedComp;
		}

		template<typename ComponentType>
		inline bool RemoveComponent(std::shared_ptr<ECS::Entity>& entity)
		{
			static_assert(std::is_base_of<ECS::BaseComponent, ComponentType>::value,
				"ComponentType must inherit from BaseComponent");

			ECS::ComponentID cId = ECS::getComponentTypeID<ComponentType>();

			FY_CORE_ASSERT(entity->Bitset[cId], "Entity({0}) doesn't have this component.", entity->Id);

			// Remove from component manager
			m_ComponentManagers[cId]->Remove(entity);

			// Remove from system(s)

			// Remove from entity's bitset
			entity->Bitset[cId] = false;

			return true;
		}

	private:
		template<typename ComponentType>
		inline ECS::ComponentManager<ComponentType>* GetComponentManager()
		{
			return dynamic_cast<ECS::ComponentManager<ComponentType>*>(m_ComponentManagers[ECS::getComponentTypeID<ComponentType>()].get());
		}

	private:
		std::unique_ptr<ECS::EntityManager> m_EntityManager;
		std::array<std::unique_ptr<ECS::BaseComponentManager>, ECS::MAX_COMPONENTS> m_ComponentManagers;

	};

}

#endif // !SCENE_HPP

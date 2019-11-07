#ifndef SCENE_HPP
#define SCENE_HPP

#include "Frosty/Core/ECS.hpp"

namespace Frosty
{
	class Scene
	{
	public:
		Scene() = default;
		Scene(const Scene& org) { FY_CORE_ASSERT(false, "Copy constructor in Scene called."); }
		virtual ~Scene() = default;

		// Operators
		Scene& operator=(const Scene& org) { FY_CORE_ASSERT(false, "Assignment operator in Scene called."); return *this; }

		void Init();
		void AddCamera(std::shared_ptr<ECS::Entity>& camera);
		inline const std::shared_ptr<ECS::Entity>& GetCamera() const { return m_GameCamera; }

		// Entity Functions
		std::shared_ptr<ECS::Entity>& CreateEntity(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool isStatic);
		void RemoveEntity(std::shared_ptr<ECS::Entity>& entity);
		inline std::unique_ptr<ECS::EntityManager>& GetEntityManager() { return m_EntityManager; }
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

			return addedComp;
		}

		template<typename ComponentType>
		inline void RemoveComponent(std::shared_ptr<ECS::Entity>& entity)
		{
			static_assert(std::is_base_of<ECS::BaseComponent, ComponentType>::value,
				"ComponentType must inherit from BaseComponent");

			ECS::ComponentID cId = ECS::getComponentTypeID<ComponentType>();

			FY_CORE_ASSERT(entity->Bitset[cId], "Entity({0}) doesn't have this component.", entity->Id);

			// Remove from component manager
			m_ComponentManagers[cId]->Remove(entity);

			// Remove from entity's bitset
			entity->Bitset[cId] = false;
		}
		
		template<typename ComponentType>
		inline ECS::ComponentManager<ComponentType>* GetComponentManager()
		{
			return dynamic_cast<ECS::ComponentManager<ComponentType>*>(m_ComponentManagers[ECS::getComponentTypeID<ComponentType>()].get());
		}

	private:
		// Entity Declarations
		std::shared_ptr<ECS::Entity> m_GameCamera;
		std::unique_ptr<ECS::EntityManager> m_EntityManager;

		// Component Declarations
		std::array<std::unique_ptr<ECS::BaseComponentManager>, ECS::MAX_COMPONENTS> m_ComponentManagers;

	};

}

#endif // !SCENE_HPP

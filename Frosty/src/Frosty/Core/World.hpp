#ifndef WORLD_HPP
#define WORLD_HPP

#include "Frosty/API/Scene.hpp"

namespace Frosty
{
	class World
	{
	public:
		World() = default;
		World(const World& org) { FY_CORE_ASSERT(false, "Copy constructor in World called."); }
		virtual ~World() = default;

		// Operators
		World& operator=(const World& org) { FY_CORE_ASSERT(false, "Assignment operator in World called."); return *this; }

		// System Functions
		void Init();
		void Start();
		void OnInput();
		void OnUpdate();
		void BeginScene(bool editorCamera);
		void Render();

		template<typename SystemType>
		inline void AddSystem()
		{
			SystemType* system(new SystemType());
			std::unique_ptr<SystemType> systemPtr{ system };
			m_Systems[m_TotalSystems] = std::move(systemPtr);
			//m_Systems[m_TotalSystems]->attachWorld(this);
			m_Systems[m_TotalSystems++]->Init();
		}

		// Scene Functions
		std::unique_ptr<Scene>& CreateScene();
		void DestroyScene();

		// Entity Functions
		inline std::shared_ptr<ECS::Entity>& CreateEntity() { return m_Scene->CreateEntity(); }
		void RemoveEntity(std::shared_ptr<ECS::Entity>& entity);
		inline std::unique_ptr<ECS::EntityManager>& GetEntityManager() { return m_Scene->GetEntityManager(); }
		template<typename ComponentType>
		inline bool HasComponent(const std::shared_ptr<ECS::Entity>& entity)
		{
			return m_Scene->HasComponent<ComponentType>(entity);
		}

		// Component Functions
		template<typename ComponentType>
		inline ComponentType& GetComponent(const std::shared_ptr<ECS::Entity>& entity)
		{
			return m_Scene->GetComponent<ComponentType>(entity);
		}

		template<typename ComponentType, typename... TArgs>
		inline ComponentType& AddComponent(std::shared_ptr<ECS::Entity>& entity, TArgs&&... mArgs)
		{
			//FY_CORE_ASSERT(cId > -1, "Component was not found!");
			ComponentType& addedComp = m_Scene->AddComponent<ComponentType>(entity, std::forward<TArgs>(mArgs)...);

			for (size_t i = 1; i < m_TotalSystems; i++)
			{
				m_Systems[i]->AddComponent(entity);
			}

			return addedComp;
		}

		template<typename ComponentType>
		inline void RemoveComponent(std::shared_ptr<ECS::Entity>& entity)
		{
			m_Scene->RemoveComponent<ComponentType>(entity);

			for (size_t i = 1; i < m_TotalSystems; i++)
			{
				if (m_Systems[i]->GetSignature().test(ECS::getComponentTypeID<ComponentType>()))
				{
					m_Systems[i]->RemoveEntity(entity);
				}
			}
		}

		template<typename ComponentType>
		inline void InitiateComponent()
		{
			ComponentType* component(new ComponentType());
			std::unique_ptr<ComponentType> componentPtr{ component };
			m_ComponentList[ECS::getComponentTypeID<ComponentType>()] = std::move(componentPtr);
		}

	private:
		// Scene Declarations
		std::unique_ptr<Scene> m_Scene;

		// System Declarations 
		std::array<std::unique_ptr<ECS::BaseSystem>, ECS::MAX_SYSTEMS> m_Systems;
		size_t m_TotalSystems{ 1 };

		// Component Declarations
		//std::array<std::unique_ptr<ECS::BaseComponent>, ECS::MAX_COMPONENTS> m_ComponentList;
		//size_t m_TotalComponents{ 0 };

	};
}

#endif // !WORLD_HPP

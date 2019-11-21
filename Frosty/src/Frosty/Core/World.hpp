#ifndef WORLD_HPP
#define WORLD_HPP

#include "Frosty/API/Scene.hpp"
#include "Frosty/StateMachine/StateMachine.hpp"

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
		void Awake();
		void OnStart();
		void OnInput();
		void OnUpdate();
		void OnEvent(BaseEvent& e);
		void BeginScene();
		void Render();

		// System Functions
		template<typename SystemType>
		inline ECS::BaseSystem* AddSystem()
		{ 
			SystemType* system(FY_NEW SystemType());
			std::unique_ptr<SystemType> systemPtr{ system };
			m_Systems[m_TotalSystems] = std::move(systemPtr);
			m_TotalSystems++;
			return m_Systems[m_TotalSystems - 1].get();
		}
		
		void UpdateSystems(const std::shared_ptr<ECS::Entity>& entity);

		// Scene Functions
		std::unique_ptr<Scene>& CreateScene();
		void DestroyScene();
		// Returns the camera entity for the scene (later change this when having multiple scenes)
		const std::shared_ptr<ECS::Entity>& GetSceneCamera() const { return m_Scene->GetCamera(); }
		void SetSceneCamera(const std::shared_ptr<ECS::Entity>& entity);
		
		//Rooms / Groups
		void DestroyGroup(bool current = true);
		void HandleDestroyedRoom();
		size_t GetCurrentRoom() const;
		void ChangeCurrentRoom();

		// Entity Functions (Room/Groups)
		void AddToGroup(const std::shared_ptr<ECS::Entity>& entity, bool current = true);

		// Entity Functions
		std::shared_ptr<ECS::Entity>& CreateEntity(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f), bool isStatic = false);
		void RemoveEntity(const std::shared_ptr<ECS::Entity>& entity);
		inline std::unique_ptr<ECS::EntityManager>& GetEntityManager() { return m_Scene->GetEntityManager(); }
		template<typename ComponentType>
		inline bool HasComponent(const std::shared_ptr<ECS::Entity>& entity)
		{
			return m_Scene->HasComponent<ComponentType>(entity);
		}
		void AddToDestroyList(const std::shared_ptr<ECS::Entity>& entity);
		
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
		inline ComponentType* GetComponentAddress(const std::shared_ptr<ECS::Entity>& entity)
		{
			return m_Scene->GetComponentAddress<ComponentType>(entity);
		}

		template<typename ComponentType>
		inline ECS::ComponentManager<ComponentType>* GetComponentManager()
		{
			return m_Scene->GetComponentManager<ComponentType>();
		}
		
		// Other
		void PrintWorld();

	private:
		void HandleDestroyedEntities();

	private:
		// Scene Declarations
		std::unique_ptr<Scene> m_Scene;
		size_t m_CurrentRoom{ 0 };

		// System Declarations 
		std::array<std::unique_ptr<ECS::BaseSystem>, ECS::MAX_SYSTEMS> m_Systems;
		size_t m_TotalSystems{ 1 };

		std::vector<std::shared_ptr<ECS::Entity>> m_DestroyedEntities;
		int32_t m_DestroyRoom{ -1 };

	};
}

#endif // !WORLD_HPP

#ifndef ECS_HPP
#define ECS_HPP

#include <fypch.hpp>

namespace Frosty
{
	namespace ECS
	{
#pragma region Declarations
		using EntityID = size_t;
		using ComponentID = size_t;
#pragma endregion


#pragma region Settings
#pragma endregion


#pragma region Entity

		struct Entity
		{
			inline static EntityID s_LastId{ 0 };
			EntityID Id;

			Entity() : Id(s_LastId++) { FY_CORE_INFO("An entity({0}) was successfully created.", Id); }
			Entity(const Entity& obj) { FY_CORE_ASSERT(false, "Copy constructor in Entity called."); }
			virtual ~Entity() { }

			// Operators
			Entity& operator=(const Entity& e) { FY_CORE_ASSERT(false, "Assignment operator in Entity called."); }
		};

		class EntityManager
		{
		public:
			EntityManager() { }
			EntityManager(const EntityManager& obj) { FY_CORE_ASSERT(false, "Copy constructor in EntityManager called."); }
			virtual ~EntityManager() { }

			// Operators
			EntityManager& operator=(const EntityManager& e) { FY_CORE_ASSERT(false, "Assignment operator in EntityManager called."); }

			inline std::shared_ptr<Entity>& CreateEntity()
			{
				FY_CORE_INFO("Creating a new entity..");
				m_Entities.emplace_back(new Entity());
				return m_Entities.back();
			}

			inline bool RemoveEntity(const std::shared_ptr<Entity>& entity)
			{
				FY_CORE_INFO("Removing an entity..");

			}

		private:
			std::vector<std::shared_ptr<Entity>> m_Entities;

		};

#pragma endregion


#pragma region Component
#pragma endregion
	}
}

#endif // !ECS_HPP

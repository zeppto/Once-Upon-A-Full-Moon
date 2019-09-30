#ifndef ECS_HPP
#define ECS_HPP

namespace Frosty
{
	namespace utils
	{
		template<typename T, typename C>
		inline int BinarySearch(const std::vector<std::shared_ptr<T>>& v, const C& value)
		{
			unsigned int first = 0;
			unsigned int last = (unsigned int)(v.size() - 1);
			unsigned int middle = 0;
			int index = -1;
			unsigned int bSearchLimit = 5;

			while (index == -1 && last >= first)
			{
				middle = (last + first) / 2;

				if (*v[middle] == value)
				{
					// Value Found
					index = middle;
				}
				else if (*v[middle] > value)
				{
					// Value is less. Not found
					// Move last
					last = middle - 1;
				}
				else if (*v[middle] < value)
				{
					// Value is greater. Not found
					// Move first
					first = middle + 1;
				}

				if ((last - first) <= bSearchLimit)
				{
					// Start linear search instead
					for (unsigned int i = first; i <= last && index == -1; i++)
					{
						if (*v[i] == value)
						{
							index = i;
						}
					}
				}
			}

			return index;
		}

		template<size_t N>
		inline bool BitsetFits(const std::bitset<N>& key, const std::bitset<N>& keyHole)
		{
			if (key.count() > keyHole.count())
			{
				return false;
			}

			bool fits = true;

			for (unsigned int i = 0; i < N && fits; i++)
			{
				if (key[i] && !keyHole[i])
				{
					fits = false;
				}
			}

			return fits;
		}
	}

	namespace ECS
	{
#pragma region Settings

		// Let's define a maximum number of unique components:
		constexpr std::size_t MAX_COMPONENTS{ 32 };

		// Let's define a maximum number of entities that
		// can have the same component type:
		constexpr std::size_t MAX_ENTITIES_PER_COMPONENT{ 1024 };

#pragma endregion


#pragma region Declarations

		struct BaseComponent;

		using EntityID = size_t;				// Unique ID of the entity
		using ComponentID = size_t;				// Unique ID of the component type (not component itself)
		using ComponentArrayIndex = size_t;		// An index to where in the component array the component is stored for a specific entity
		using ComponentBitset = std::bitset<MAX_COMPONENTS>;

#pragma endregion


#pragma region Utilities

		//struct ComponentInfo
		//{
		//	std::unordered_map<ComponentID, std::string> s_ComponentNames;
		//
		//	inline std::string& GetComponentNameById(ComponentID cid)
		//	{
		//		FY_CORE_ASSERT(!s_ComponentNames[cid].empty(), "Can't retrieve information about a component before it has been added once at least.");
		//		return s_ComponentNames[cid];
		//	}
		//
		//	inline void SetComponentNameById(ComponentID cid, const std::string& name)
		//	{
		//		if (s_ComponentNames[cid].empty())
		//			s_ComponentNames[cid] = name;
		//	}
		//};

		namespace Internal
		{
			inline ComponentID getComponentUniqueID()
			{
				static ComponentID lastCID{ 0u };
				return lastCID++;
			}
		}

		template <typename ComponentType>
		inline ComponentID getComponentTypeID() noexcept
		{
			static_assert(std::is_base_of<BaseComponent, ComponentType>::value,
				"ComponentType must inherit from BaseComponent");

			static ComponentID typeCID{ Internal::getComponentUniqueID() };
			//ComponentInfo::SetComponentNameById(typeCID, ComponentType::NAME);

			return typeCID;
		}

#pragma endregion


#pragma region Entity

		struct Entity
		{
			inline static EntityID s_LastId{ 0 };
			EntityID Id;
			ComponentBitset Bitset;

			Entity() : Id(s_LastId++) { FY_CORE_INFO("An entity({0}) was successfully created.", Id); }
			Entity(const Entity& obj) { FY_CORE_ASSERT(false, "Copy constructor in Entity called."); }
			virtual ~Entity() { FY_CORE_INFO("An entity({0}) was successfully deleted.", Id); }

			// Operators
			Entity& operator=(const Entity& e) { FY_CORE_ASSERT(false, "Assignment operator in Entity called."); return *this; }
			bool operator==(const EntityID& mid) const { return Id == mid; }
			bool operator>(const EntityID& mid) const { return Id > mid; }
			bool operator<(const EntityID& mid) const { return Id < mid; }
			bool operator<=(const EntityID& mid) const { return Id <= mid; }
			bool operator>=(const EntityID& mid) const { return Id >= mid; }

			friend bool operator<(const Entity& lhs, const Entity& rhs) { return lhs.Id < rhs.Id; }
			friend bool operator>(const Entity& lhs, const Entity& rhs) { return lhs.Id > rhs.Id; }
			friend bool operator==(const Entity& lhs, const Entity& rhs) { return lhs.Id == rhs.Id; }
			friend bool operator!=(const Entity& lhs, const Entity& rhs) { return lhs.Id != rhs.Id; }
			friend bool operator>=(const Entity& lhs, const Entity& rhs) { return lhs.Id >= rhs.Id; }
			friend bool operator<=(const Entity& lhs, const Entity& rhs) { return lhs.Id <= rhs.Id; }

			friend std::ostream& operator<<(std::ostream& out, const Entity& e)
			{
				out << "Entity(#" << e.Id << ") has the following bitset:\n" << e.Bitset;
				return out;
			}

		};

		class EntityManager
		{
		public:
			EntityManager() { }
			EntityManager(const EntityManager& obj) { FY_CORE_ASSERT(false, "Copy constructor in EntityManager called."); }
			virtual ~EntityManager() { }

			// Operators
			EntityManager& operator=(const EntityManager& e) { FY_CORE_ASSERT(false, "Assignment operator in EntityManager called."); return *this; }

			inline size_t GetTotalEntities() const { return m_Entities.size(); }

			inline const std::shared_ptr<Entity>& At(size_t index) const { return m_Entities.at(index); }

			inline std::shared_ptr<Entity>& Create()
			{
				FY_CORE_INFO("Creating a new entity..");
				m_Entities.emplace_back(FY_NEW Entity());
				return m_Entities.back();
			}

			inline bool Remove(std::shared_ptr<Entity>& entity)
			{
				FY_CORE_INFO("Removing an entity..");

				int index = utils::BinarySearch(m_Entities, entity->Id);

				if (index == -1)
				{
					return false;
				}

				m_Entities.erase(m_Entities.begin() + index);

				return true;
			}

			friend std::ostream& operator<<(std::ostream& out, const EntityManager& em)
			{
				for (unsigned int i = 0; i < em.m_Entities.size(); i++)
				{
					out << *em.m_Entities[i] << std::endl;
				}
				return out;
			}

		private:
			std::vector<std::shared_ptr<Entity>> m_Entities;

		};

#pragma endregion


#pragma region Component

		struct BaseComponent
		{
			std::shared_ptr<Entity> EntityPtr;
		};

		struct BaseComponentManager
		{
			ComponentID TypeId;

			std::map<std::shared_ptr<Entity>, ComponentArrayIndex> EntityMap;
			ComponentArrayIndex Total{ 1 };

			BaseComponentManager() { }
			BaseComponentManager(const BaseComponentManager& obj) { FY_CORE_ASSERT(false, "Copy constructor in BaseComponentManager called."); }
			virtual ~BaseComponentManager() { }

			// Operators
			BaseComponentManager& operator=(const BaseComponentManager& e) { FY_CORE_ASSERT(false, "Assignment operator in BaseComponentManager called."); return *this; }

			virtual void Remove(std::shared_ptr<Entity>& entity) = 0;
		};

		template<typename ComponentType>
		class ComponentManager : public BaseComponentManager
		{
		public:
			// TODO:
				// RemoveComponent() - Make sure to update the entity bitset. Either handle it here or inside EntityManager
		public:
			ComponentManager() : BaseComponentManager() { FY_CORE_INFO("A new component manager({0}) was successfully created.", getComponentTypeID<ComponentType>()); }
			ComponentManager(const ComponentManager& obj) : BaseComponentManager(obj) { FY_CORE_ASSERT(false, "Copy constructor in ComponentManager({0}) called.", getComponentTypeID<ComponentType>()); }
			virtual ~ComponentManager() { }

			// Operators
			ComponentManager& operator=(const ComponentManager& e) { FY_CORE_ASSERT(false, "Assignment operator in ComponentManager({0}) called.", getComponentTypeID<ComponentType>()); return *this; }

			inline ComponentType& Get(const std::shared_ptr<Entity>& entity)
			{
				ComponentArrayIndex tempIndex = EntityMap.at(entity);

				return m_Data[tempIndex];
			}

			template<typename... TArgs>
			inline ComponentType& Add(std::shared_ptr<Entity>& entity, TArgs&&... mArgs)
			{
				FY_CORE_ASSERT(Total < MAX_ENTITIES_PER_COMPONENT,
					"Maximum number of entities for this specific component({0}) is reached.", getComponentTypeID<ComponentType>());

				EntityMap.emplace(entity, Total);
				m_Data.at(Total) = ComponentType(std::forward<TArgs>(mArgs)...);
				entity->Bitset.flip(getComponentTypeID<ComponentType>());
				m_Data.at(Total).EntityPtr;

				return m_Data[Total++];
			}

			inline void Remove(std::shared_ptr<Entity>& entity) 
			{
				ComponentArrayIndex index = EntityMap.at(entity);

				m_Data.at(index).EntityPtr.reset();
				m_Data.at(index) = m_Data.at(Total - 1);
				m_Data.at(Total - 1) = ComponentType();

				Total--;
				if (Total > index)
				{
					EntityMap[m_Data.at(index).EntityPtr] = index;
				}

				EntityMap.erase(entity);
				entity->Bitset.flip(getComponentTypeID<ComponentType>());
			}
			
		private:
			std::array<ComponentType, MAX_ENTITIES_PER_COMPONENT> m_Data;

		};

		// List of all Components //

		struct CTransform : public BaseComponent
		{
			static std::string NAME;
			glm::vec3 Position{ 0.0f };
			glm::vec3 Rotation{ 0.0f };
			glm::vec3 Scale{ 1.0f };

			CTransform() = default;
			CTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : Position(position), Rotation(rotation), Scale(scale) { }
		};

#pragma endregion
	}
}

#endif // !ECS_HPP

#ifndef ECS_HPP
#define ECS_HPP

#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/RenderEngine/Texture.hpp"

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
		constexpr std::size_t MAX_COMPONENTS{ 4 };

		// Let's define a maximum number of entities that
		// can have the same component type:
		constexpr std::size_t MAX_ENTITIES_PER_COMPONENT{ 1024 };

		// Defining the maximum nr of systems
		constexpr std::size_t MAX_SYSTEMS{ 16 };

#pragma endregion Settings


#pragma region Declarations

		struct BaseComponent;

		using EntityID = size_t;				// Unique ID of the entity
		using ComponentID = size_t;				// Unique ID of the component type (not component itself)
		using ComponentArrayIndex = size_t;		// An index to where in the component array the component is stored for a specific entity
		using ComponentBitset = std::bitset<MAX_COMPONENTS>;

#pragma endregion Declarations


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


#pragma endregion Utilities


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

			inline std::shared_ptr<Entity>& At(size_t index) { return m_Entities.at(index); }
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

			std::vector<std::shared_ptr<Entity>>::iterator begin() { return m_Entities.begin(); }
			std::vector<std::shared_ptr<Entity>>::iterator end() { return m_Entities.end(); }

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

#pragma endregion Entity


#pragma region Component

		struct BaseComponent
		{
			std::shared_ptr<Entity> EntityPtr;

			virtual void Func() = 0;
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
			
			virtual BaseComponent* GetTypeComponent(const std::shared_ptr<Entity>& entity) = 0;

			virtual void Remove(std::shared_ptr<Entity>& entity) = 0;

			//static std::array<std::shared_ptr<BaseComponent>, MAX_COMPONENTS> s_ComponentList;

		};

		template<typename ComponentType>
		class ComponentManager : public BaseComponentManager
		{
		public:
			ComponentManager() : BaseComponentManager() { FY_CORE_INFO("A new component manager({0}) was successfully created.", getComponentTypeID<ComponentType>()); }
			ComponentManager(const ComponentManager& obj) : BaseComponentManager(obj) { FY_CORE_ASSERT(false, "Copy constructor in ComponentManager({0}) called.", getComponentTypeID<ComponentType>()); }
			virtual ~ComponentManager() { }

			// Operators
			ComponentManager& operator=(const ComponentManager& e) { FY_CORE_ASSERT(false, "Assignment operator in ComponentManager({0}) called.", getComponentTypeID<ComponentType>()); return *this; }

			virtual BaseComponent* GetTypeComponent(const std::shared_ptr<Entity>& entity) override
			{
				ComponentArrayIndex tempIndex = EntityMap.at(entity);

				return &m_Data[tempIndex];
			}

			inline ComponentType& Get(const std::shared_ptr<Entity>& entity)
			{
				ComponentArrayIndex tempIndex = EntityMap.at(entity);

				return m_Data[tempIndex];
			}

			inline std::array<ComponentType, MAX_ENTITIES_PER_COMPONENT>& GetAll() { return m_Data; }
			inline const std::array<ComponentType, MAX_ENTITIES_PER_COMPONENT>& GetAll() const { return m_Data; }

			template<typename... TArgs>
			inline ComponentType& Add(std::shared_ptr<Entity>& entity, TArgs&&... mArgs)
			{
				FY_CORE_ASSERT(Total < MAX_ENTITIES_PER_COMPONENT,
					"Maximum number of entities for this specific component({0}) is reached.", getComponentTypeID<ComponentType>());

				EntityMap.emplace(entity, Total);
				m_Data.at(Total) = ComponentType(std::forward<TArgs>(mArgs)...);
				entity->Bitset.flip(getComponentTypeID<ComponentType>());
				m_Data.at(Total).EntityPtr = entity;

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
			bool UpdateTransform{ false };				// Not sure if this is needed outside the editor

			CTransform() = default;
			CTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : Position(position), Rotation(rotation), Scale(scale) { }
			CTransform(const CTransform& org) { FY_CORE_ASSERT(false, "Copy constructor in CTransform called."); }

			virtual void Func() override { }
		};

		struct CMesh : public BaseComponent
		{
			static std::string NAME;
			std::shared_ptr<VertexArray> Mesh;

			CMesh() = default;
			CMesh(std::shared_ptr<VertexArray> mesh) : Mesh(mesh) { }
			CMesh(const CMesh& org) { FY_CORE_ASSERT(false, "Copy constructor in CMesh called."); }

			virtual void Func() override { }
		};

		struct CCamera : public BaseComponent
		{
			static std::string NAME;
			glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
			float FieldOfView{ 60.0f };
			float Near{ 0.03f };
			float Far{ 1000.0f };
			glm::mat4 ViewMatrix{ 1.0f };
			glm::mat4 ProjectionMatrix{ 1.0f };
			glm::mat4 ViewProjectionMatrix{ 1.0f };
			bool UpdateProjection{ false };			// Not sure if this is needed outside the editor

			CCamera() = default;
			CCamera(const glm::vec3& position, float fov, float aspect, float zNear, float zFar)
				: FieldOfView(fov), Near(zNear), Far(zFar), ProjectionMatrix(glm::perspective(glm::radians(fov), aspect, zNear, zFar)),
				ViewMatrix(glm::lookAt(position, position + Front, glm::vec3(0.0f, 1.0f, 0.0f))), ViewProjectionMatrix(ProjectionMatrix * ViewMatrix)
			{
			}
			CCamera(const CCamera& org) { FY_CORE_ASSERT(false, "Copy constructor in CCamera called."); }

			virtual void Func() override { }

		};

		struct CMaterial : public BaseComponent
		{
			static std::string NAME;
			std::shared_ptr<Shader> UseShader;
			glm::vec4 Albedo{ 1.0f, 0.0f, 1.0f, 1.0f };
			std::shared_ptr<Texture2D> DiffuseTexture;
			std::shared_ptr<Texture2D> GlossTexture;
			std::shared_ptr<Texture2D> NormalTexture;

			CMaterial() = default;
			CMaterial(const std::shared_ptr<Shader>& shader) : UseShader(shader) { }
			CMaterial(const CMaterial& org) { FY_CORE_ASSERT(false, "Copy constructor in CMaterial called."); }

			virtual void Func() override { }
		};

		static std::string GetComponentName(size_t i)
		{
			switch (i)
			{
			case 0:		return "Transform";
			case 1:		return "Mesh";
			case 2:		return "Camera";
			case 3:		return "Material";
			default:	return "";
			}
		}

#pragma endregion Component


#pragma region System

		class BaseSystem
		{
		public:
			BaseSystem() = default;
			virtual ~BaseSystem() = default;

			virtual void Init() = 0;
			virtual void OnInput() { }
			virtual void OnUpdate() = 0;
			virtual void BeginScene(bool editorCamera) { }
			virtual void Render() { }

			virtual void AddComponent(const std::shared_ptr<Entity>& entity) = 0;
			virtual void RemoveEntity(const std::shared_ptr<Entity>& entity) = 0;


		protected:
			ComponentBitset p_Signature;
			std::map<std::shared_ptr<Entity>, ComponentArrayIndex> p_EntityMap;
			ComponentArrayIndex p_Total{ 1 };

		private:
		};

		/* Specific Systems */
		/*
		class CameraSystem : public BaseSystem
		{
		public:
			CameraSystem() = default;
			virtual ~CameraSystem() = default;

			inline virtual void Init() override
			{
				p_Signature.set(getComponentTypeID<CTransform>(), true);
				p_Signature.set(getComponentTypeID<CCamera>(), true);
			}

			virtual void OnInput() override;
			virtual void OnUpdate() override;

			virtual void BeginScene(bool editorCamera) override
			{
				if (editorCamera) return;

				if (p_Total > 1)
				{
					RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
					RenderCommand::Clear();
					for (size_t i = 1; i < p_Total; i++)
					{
						Renderer::BeginScene(m_Cameras[i]->ViewProjectionMatrix);
					}
				}
				else
				{
					RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
					RenderCommand::Clear();
					Renderer::BeginScene(glm::mat4(1.0f));
				}
			}

			virtual void AddComponent(const std::shared_ptr<Entity>& entity) override;
			inline virtual void RemoveEntity(const std::shared_ptr<Entity>& entity) override
			{
				ComponentArrayIndex tempIndex = p_EntityMap[entity];

				if (tempIndex > 0)
				{
					p_Total--;
					m_Transform[p_Total] = nullptr;
					m_Cameras[p_Total] = nullptr;

					//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);

					if (p_Total > tempIndex)
					{
						std::shared_ptr<Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;
						p_EntityMap[entityToUpdate] = tempIndex;
					}

					p_EntityMap.erase(entity);
				}
			}

		private:
			std::array<CTransform*, MAX_ENTITIES_PER_COMPONENT> m_Transform;
			std::array<CCamera*, MAX_ENTITIES_PER_COMPONENT> m_Cameras;

		};

		class RenderSystem : public BaseSystem
		{
		public:
			RenderSystem() = default;
			virtual ~RenderSystem() = default;

			inline virtual void Init() override
			{
				p_Signature.set(getComponentTypeID<CTransform>(), true);
				p_Signature.set(getComponentTypeID<CMesh>(), true);
				p_Signature.set(getComponentTypeID<CMaterial>(), true);
			}

			inline virtual void OnUpdate() override { }

			inline virtual void Render()
			{
				for (size_t i = 1; i < p_Total; i++)
				{
					// So the question is, calculate it every frame or save the result in the component
					// and update it only when Position/Rotation/Scale changes
					// Takes up performance if calculated every frame, but the other way will mean
					// 4 * 4 floats which is 4 * 4 * 4 = 64 bytes for EVERY entity
					// For now we calculate it every frame
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Transform[i]->Position);
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.x), { 1.0f, 0.0f, 0.0f });
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.y), { 0.0f, 1.0f, 0.0f });
					transform = glm::rotate(transform, glm::radians(m_Transform[i]->Rotation.z), { 0.0f, 0.0f, 1.0f });
					transform = glm::scale(transform, m_Transform[i]->Scale);

					if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->DiffuseTexture) m_Materials[i]->DiffuseTexture->Bind(); 
					if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->GlossTexture) m_Materials[i]->GlossTexture->Bind(1);
					if (m_Materials[i]->UseShader->GetName() == "Texture2D" && m_Materials[i]->NormalTexture) m_Materials[i]->NormalTexture->Bind(2);

					Renderer::Submit(m_Materials[i], m_Meshes[i]->Mesh, transform);
				}
			}

			virtual void AddComponent(const std::shared_ptr<Entity>& entity) override;
			inline virtual void RemoveEntity(const std::shared_ptr<Entity>& entity) override
			{
				ComponentArrayIndex tempIndex = p_EntityMap[entity];

				if (tempIndex > 0)
				{
					p_Total--;
					m_Transform[p_Total] = nullptr;
					m_Meshes[p_Total] = nullptr;
					m_Materials[p_Total] = nullptr;

					if (p_Total > 1)
					{
						//std::shared_ptr<Entity> entityToUpdate = removeEntityFromData(mEntity);
						std::shared_ptr<Entity> entityToUpdate = m_Transform[p_EntityMap[entity]]->EntityPtr;

						if (p_Total > tempIndex)
						{
							p_EntityMap[entityToUpdate] = tempIndex;
						}
					}

					p_EntityMap.erase(entity);
				}
			}

		private:
			std::array<CTransform*, MAX_ENTITIES_PER_COMPONENT> m_Transform;
			std::array<CMesh*, MAX_ENTITIES_PER_COMPONENT> m_Meshes;
			std::array<CMaterial*, MAX_ENTITIES_PER_COMPONENT> m_Materials;

		};*/

#pragma endregion System

	}
}

#endif // !ECS_HPP

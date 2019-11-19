#ifndef ECS_HPP
#define ECS_HPP

#include "Frosty/RenderEngine/Renderer.hpp"
#include "Frosty/RenderEngine/Texture.hpp"
#include "Frosty/Core/KeyCodes.h"
#include "Frosty/Core/MouseButtonCodes.h"

#include "Frosty/UI/UIText.h"
#include "Frosty/UI/UISprite.h"
#include "Frosty/UI/UILayout.hpp"

#include <Luna/include/Luna.h>

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
		constexpr std::size_t MAX_COMPONENTS{ 22 };

		// Let's define a maximum number of entities that
		// can have the same component type:
		constexpr std::size_t MAX_ENTITIES_PER_COMPONENT{ 1024 };

		// Defining the maximum nr of systems
		constexpr std::size_t MAX_SYSTEMS{ 20 };

#pragma endregion Settings


#pragma region Declarations

		struct BaseComponent;

		using EntityID = size_t;				// Unique ID of the entity
		using ComponentID = size_t;				// Unique ID of the component type (not component itself)
		using ComponentArrayIndex = size_t;		// An index to where in the component array the component is stored for a specific entity
		using ComponentBitset = std::bitset<MAX_COMPONENTS>;

#pragma endregion Declarations


#pragma region Utilities

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
			inline static EntityID s_LastId{ 1 };
			EntityID Id{ 0 };
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
			EntityManager() { m_Entities.reserve(MAX_ENTITIES_PER_COMPONENT); }
			EntityManager(const EntityManager& obj) { FY_CORE_ASSERT(false, "Copy constructor in EntityManager called."); }
			virtual ~EntityManager() { }

			// Operators
			EntityManager& operator=(const EntityManager& e) { FY_CORE_ASSERT(false, "Assignment operator in EntityManager called."); return *this; }

			inline std::vector<std::shared_ptr<Entity>>& GetEntities() { return m_Entities; }
			inline std::shared_ptr<Entity>& GetEntityById(EntityID eid)
			{
				int index = utils::BinarySearch(m_Entities, eid);
				return m_Entities[index];
			}
			inline size_t GetTotalEntities() const { return m_Entities.size(); }

			inline std::shared_ptr<Entity>& At(size_t index) { return m_Entities.at(index); }
			inline const std::shared_ptr<Entity>& At(size_t index) const { return m_Entities.at(index); }

			inline std::shared_ptr<Entity>& Create()
			{
				FY_CORE_INFO("Creating a new entity..");
				m_Entities.emplace_back(FY_NEW Entity());
				return m_Entities.back();
			}

			inline bool Remove(const std::shared_ptr<Entity>& entity)
			{
				FY_CORE_INFO("Removing an entity..");

				int index = utils::BinarySearch(m_Entities, entity->Id);

				FY_CORE_ASSERT(index >= 0, "Entity doesn't exist in the manager.");
				//if (index == -1)
				//{
				//	return false;
				//}

				m_Entities.erase(m_Entities.begin() + index);

				return true;
			}

			std::vector<std::shared_ptr<Entity>>::iterator begin() { return m_Entities.begin(); }
			std::vector<std::shared_ptr<Entity>>::iterator end() { return m_Entities.end(); }

			friend std::ostream& operator<<(std::ostream& out, const EntityManager& em)
			{
				out << "\t-----------Entity Info-----------\n";
				out << "\tIndex\tId\tAddress\t\t\tRefs\n";
				for (unsigned int i = 0; i < em.m_Entities.size(); i++)
				{
					out << "\t" << i << "\t" << em.m_Entities.at(i)->Id << "\t" << em.m_Entities.at(i) << "\t" << em.m_Entities.at(i).use_count() << std::endl;
				}
				out << "\t----------------Done----------------\n\n";

				return out;
			}

		private:
			std::vector<std::shared_ptr<Entity>> m_Entities;

		};

#pragma endregion Entity


#pragma region Component

		struct BaseComponent
		{
			std::shared_ptr<Entity> EntityPtr{ nullptr };

			virtual std::string GetName() const = 0;
		};

		struct BaseComponentManager
		{
			ComponentID TypeId{ 0 };
			std::map<std::shared_ptr<Entity>, ComponentArrayIndex> EntityMap;

			ComponentArrayIndex Total{ 1 };

			BaseComponentManager() { }
			BaseComponentManager(const BaseComponentManager& obj) { FY_CORE_ASSERT(false, "Copy constructor in BaseComponentManager called."); }
			virtual ~BaseComponentManager() { }

			// Operators
			BaseComponentManager& operator=(const BaseComponentManager& e) { FY_CORE_ASSERT(false, "Assignment operator in BaseComponentManager called."); return *this; }

			//inline ComponentArrayIndex GetTotal() const { return Total; }
			virtual BaseComponent* GetTypeComponent(const std::shared_ptr<Entity>& entity) = 0;
			virtual const std::shared_ptr<Entity>& Remove(const std::shared_ptr<Entity>& entity) = 0;
			virtual std::string GetInfo() const = 0;

			friend std::ostream& operator<<(std::ostream& out, const BaseComponentManager& bcm)
			{
				return out << bcm.GetInfo();
			}

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

			inline ComponentArrayIndex GetTotal() { return Total; }
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
			inline ComponentType& Add(std::shared_ptr<Entity>& entity, TArgs&& ... mArgs)
			{
				FY_CORE_ASSERT(Total < MAX_ENTITIES_PER_COMPONENT,
					"Maximum number of entities for this specific component({0}) is reached.", getComponentTypeID<ComponentType>());

				EntityMap.emplace(entity, Total);
				m_Data.at(Total) = ComponentType(std::forward<TArgs>(mArgs)...);
				entity->Bitset.flip(getComponentTypeID<ComponentType>());
				m_Data.at(Total).EntityPtr = entity;

				return m_Data[Total++];
			}

			inline const std::shared_ptr<Entity>& Remove(const std::shared_ptr<Entity>& entity)
			{
				auto& it = EntityMap.find(entity);

				FY_CORE_ASSERT(it != EntityMap.end(), "Entity doesn't exist in this component manager but we are still trying to access it.");

				ComponentArrayIndex index = it->second;

				m_Data.at(index).EntityPtr.reset();
				m_Data.at(index) = m_Data.at(Total - 1);
				m_Data.at(index).EntityPtr = m_Data.at(Total - 1).EntityPtr;
				m_Data.at(Total - 1).EntityPtr.reset();
				m_Data.at(Total - 1) = ComponentType();

				Total--;
				if (Total > index)
				{
					auto& itUpdate = EntityMap.find(m_Data.at(index).EntityPtr);
					FY_CORE_ASSERT(itUpdate != EntityMap.end(), "This should not happen!");
					EntityMap[itUpdate->first] = index;
					//EntityMap[m_Data.at(it->second).EntityPtr] = it->second;
				}

				EntityMap.erase(entity);
				entity->Bitset.flip(getComponentTypeID<ComponentType>());
				return m_Data.at(index).EntityPtr;
			}

			inline ComponentType* GetComponentAddress(const std::shared_ptr<Entity>& entity)
			{
				return &m_Data.at(EntityMap[entity]);
			}

			virtual std::string GetInfo() const override
			{
				std::stringstream retInfo;
				retInfo << "\t-----------" << m_Data[0].GetName() << " Component Manager-----------\n";
				retInfo << "\tIndex\tComponent Address\tEntity Id\tEntity Address\t\tEntity Refs\n";
				for (size_t i = 1; i < Total; i++)
				{
					retInfo << "\t" << i << "\t" << &m_Data[i] << "\t" << m_Data[i].EntityPtr->Id << "\t\t" << m_Data[i].EntityPtr << "\t\t" << m_Data[i].EntityPtr.use_count() << std::endl;
				}
				retInfo << "\t----------------Done----------------\n\n";

				return retInfo.str();
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
			glm::mat4 ModelMatrix{ 1.0f };
			bool IsStatic{ false };

			CTransform() = default;
			CTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, bool isStatic = false)
				: Position(position), Rotation(rotation), Scale(scale), IsStatic(isStatic)
			{
			
				if (isStatic)
				{
					ModelMatrix = glm::translate(glm::mat4(1.0f), Position);
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.x), { 1.0f, 0.0f, 0.0f });
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.y), { 0.0f, 1.0f, 0.0f });
					ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.z), { 0.0f, 0.0f, 1.0f });
					ModelMatrix = glm::scale(ModelMatrix, Scale);
				}
			}
			CTransform(const CTransform& org) { FY_CORE_ASSERT(false, "Copy constructor in CTransform called."); }
			CTransform& operator=(const CTransform& org)
			{
				if (this != &org)
				{
					Position = org.Position;
					Rotation = org.Rotation;
					Scale = org.Scale;
					ModelMatrix = org.ModelMatrix;
					IsStatic = org.IsStatic;
				}
				return *this;
			}

			glm::mat4* GetModelMatrix()
			{
				ModelMatrix = glm::translate(glm::mat4(1.0f), Position);
				ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.x), { 1.0f, 0.0f, 0.0f });
				ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.y), { 0.0f, 1.0f, 0.0f });
				ModelMatrix = glm::rotate(ModelMatrix, glm::radians(Rotation.z), { 0.0f, 0.0f, 1.0f });
				ModelMatrix = glm::scale(ModelMatrix, Scale);

				return &ModelMatrix;

			}
			virtual std::string GetName() const { return NAME; }
		};

		struct CMesh : public BaseComponent
		{
			static std::string NAME;
			std::shared_ptr<VertexArray> Mesh;
			bool RenderMesh{ true };

			CMesh() = default;
			CMesh(std::shared_ptr<VertexArray> mesh, bool render = true) : Mesh(mesh), RenderMesh(render) { }
			CMesh(const CMesh& org) { FY_CORE_ASSERT(false, "Copy constructor in CMesh called."); }
			CMesh& operator=(const CMesh& org)
			{
				FY_WARN("Assignment operator in CMesh called.");

				if (this != &org)
				{
					Mesh = org.Mesh;
				}

				return *this;
			}
			bool operator!=(const CMesh& org) { return Mesh != org.Mesh; }

			virtual std::string GetName() const { return NAME; }
		};

		struct CCamera : public BaseComponent
		{
			static std::string NAME;
			CTransform* Target{ nullptr };
			glm::vec3 Front{ 0.0f, 0.0f, -1.0f };
			glm::vec3 Background{ 0.2f };
			float FieldOfView{ 40.0f };
			float Near{ 0.03f };
			float Far{ 1000.0f };
			glm::mat4 ViewMatrix{ 1.0f };
			glm::mat4 ProjectionMatrix{ 1.0f };
			glm::mat4 ViewProjectionMatrix{ 1.0f };
			bool UpdateProjection{ false };			// Not sure if this is needed outside the editor

			CCamera() = default;
			CCamera(float fov, float aspect, float zNear, float zFar)
				: FieldOfView(fov), Near(zNear), Far(zFar), ProjectionMatrix(glm::perspective(glm::radians(fov), aspect, zNear, zFar))
			{
			}
			CCamera(const CCamera& org) { FY_CORE_ASSERT(false, "Copy constructor in CCamera called."); }
			CCamera& operator=(const CCamera& org)
			{
				if (this != &org)
				{
					Target = org.Target;
					Front = org.Front;
					Background = org.Background;
					FieldOfView = org.FieldOfView;
					Near = org.Near;
					Far = org.Far;
					ViewMatrix = org.ViewMatrix;
					ProjectionMatrix = org.ProjectionMatrix;
					ViewProjectionMatrix = org.ViewProjectionMatrix;
					UpdateProjection = org.UpdateProjection;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CMaterial : public BaseComponent
		{
			static const unsigned int MAXIMUM_SHININESS = 256;
			static std::string NAME;
			std::shared_ptr<Shader> UseShader;
			glm::vec4 Albedo{ 1.0f, 0.0f, 1.0f, 1.0f };
			std::shared_ptr<Texture2D> DiffuseTexture;
			std::shared_ptr<Texture2D> SpecularTexture;
			std::shared_ptr<Texture2D> NormalTexture;
			std::shared_ptr<Texture2D> BlendMapTexture;
			std::shared_ptr<Texture2D> BlendTexture1;
			std::shared_ptr<Texture2D> BlendTexture2;


			float SpecularStrength{ 0.5f };
			int Shininess{ 16 };
			glm::vec2 TextureScale{ 1.0f };

			CMaterial() = default;
			CMaterial(const std::shared_ptr<Shader>& shader) : UseShader(shader) { }
			CMaterial(const CMaterial& org) { FY_CORE_ASSERT(false, "Copy constructor in CMaterial called."); }
			CMaterial& operator=(const CMaterial& org)
			{
				if (this != &org)
				{
					UseShader = org.UseShader;
					Albedo = org.Albedo;
					DiffuseTexture = org.DiffuseTexture;
					SpecularTexture = org.SpecularTexture;
					NormalTexture = org.NormalTexture;
					BlendMapTexture = org.BlendMapTexture;
					BlendTexture1 = org.BlendTexture1;
					BlendTexture2 = org.BlendTexture2;
					SpecularStrength = org.SpecularStrength;
					Shininess = org.Shininess;
					TextureScale = org.TextureScale;
				}

				return *this;
			}
			bool operator!=(const CMaterial& org) { return Albedo != org.Albedo; }	// This works best for Flatcolor shader. Talk to W-_-W if you have any questions
			virtual std::string GetName() const { return NAME; }
		};

		struct CFollow : public BaseComponent
		{
			static std::string NAME;
			CTransform* Target{ nullptr };
			float StopDistance{ 0.0f };

			CFollow() = default;
			CFollow(CTransform* target) : Target(target) { }
			CFollow(const CFollow& org) { FY_CORE_ASSERT(false, "Copy constructor in CFollow called."); }

			virtual std::string GetName() const { return NAME; }
		};

		struct CLight : public BaseComponent
		{
			enum class LightType { Point, Directional };

			static std::string NAME;
			LightType Type{ LightType::Point };
			glm::vec3 Color{ 1.0f, 0.96f, 0.84f };
			glm::vec3 Direction{ 1.0f, 0.0f, 1.0f };
			float Radius{ 20.0f };
			float Strength{ 1.0f };

			CLight() = default;
			CLight(LightType lightType) : Type(lightType) { }
			CLight(LightType lightType, float strength, glm::vec3 color, float radius, glm::vec3 direction) : Type(lightType), Strength(strength), Color(color), Radius(radius),Direction(direction) { }
			CLight(LightType lightType, float strength, glm::vec3 color) : Type(lightType), Strength(strength), Color(color) { }
			CLight(const CLight& org) { FY_CORE_ASSERT(false, "Copy constructor in CLight called."); }
			CLight& operator=(const CLight& org)
			{
				if (this != &org)
				{
					Type = org.Type;
					Color = org.Color;
					Radius = org.Radius;
					Strength = org.Strength;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CPhysics : public BaseComponent
		{
			static std::string NAME;
			std::shared_ptr<Luna::BoundingBox> BoundingBox;
			glm::vec3 Direction{ 0.0f, 0.0f, 0.0f };
			float MaxSpeed{ 100.f };							// Maximun speed an entiry can upgrate its speed to
			float Speed{ 0.0f };
			glm::vec3 Velocity{ 0.0f };
			float SpeedMultiplier{ 1.f };						// Used in combination with Speed Boost Potion

			CPhysics() = default;
			CPhysics(const std::shared_ptr<Luna::BoundingBox>& bb, float speed = 0.0f) : BoundingBox(bb), Speed(speed) { }
			CPhysics(const CPhysics& org) { FY_CORE_ASSERT(false, "Copy constructor in CPhysics called."); }
			CPhysics& operator=(const CPhysics& org)
			{
				FY_WARN("Assignment operator in CPhysics called.");

				if (this != &org)
				{
					BoundingBox = org.BoundingBox;
					Direction = org.Direction;
					MaxSpeed = org.MaxSpeed;
					Speed = org.Speed;
					Velocity = org.Velocity;
					SpeedMultiplier = org.SpeedMultiplier;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CWeapon : public BaseComponent
		{
			static std::string NAME;

			enum class WeaponType { Sword, Arrow };
			WeaponType Type{ WeaponType::Sword };

			unsigned int ItemID{ 1 };
			bool IsPlayerWeapon{ false };

			// Range
			float AttackRange{ 15.f };						// 6.f if melee and 25.f if bow

			// Damage
			float Damage{ 1.0f };

			// Critical Hit
			float CriticalHit{ 0.5f };						// Adds upp with damage for total damage
			float CriticalHitChance{ 0.1f };				// 10 % chanse of performing a critical hit

			// Speed
			float LVL1AttackCooldown{ 1.f };
			float LVL2AttackCooldown{ 2.f };
			float LVL3AttackCooldown{ 3.f };

			float LVL1AttackCooldownTimer{ Frosty::Time::CurrentTime() };
			float LVL2AttackCooldownTimer{ Frosty::Time::CurrentTime() };
			float LVL3AttackCooldownTimer{ Frosty::Time::CurrentTime() };

			float Lifetime{ 2.f };

			// Special Effect / Elemental Abilities
			// Fire (+ CriticalChance)
			// Earth (+ Damage)
			// Wind (+ Speed)
			// Water (+ Heal)

			CWeapon() = default;
			CWeapon(WeaponType type, unsigned int itemID, float damage, bool isPlayerWeapon = false) : Type(type), ItemID(itemID), Damage(damage), IsPlayerWeapon(isPlayerWeapon) { }
			CWeapon(const CWeapon& org) { FY_CORE_ASSERT(false, "Copy constructor in CWeapon called."); }
			CWeapon& operator=(const CWeapon& org)
			{
				if (this != &org)
				{
					Type = org.Type;
					ItemID = org.ItemID;
					AttackRange = org.AttackRange;
					Damage = org.Damage;
					CriticalHit = org.CriticalHit;
					CriticalHitChance = org.CriticalHitChance;
					LVL1AttackCooldown = org.LVL1AttackCooldown;
					LVL2AttackCooldown = org.LVL2AttackCooldown;
					LVL3AttackCooldown = org.LVL3AttackCooldown;
					LVL1AttackCooldownTimer = org.LVL1AttackCooldownTimer;
					LVL2AttackCooldownTimer = org.LVL2AttackCooldownTimer;
					LVL3AttackCooldownTimer = org.LVL3AttackCooldownTimer;
					Lifetime = org.Lifetime;
				}


				return *this;
			}
			bool operator!=(const CWeapon& org) { return ItemID != org.ItemID; }
			virtual std::string GetName() const { return NAME; }
		};

		struct CAttack : public BaseComponent
		{
			static std::string NAME;

			enum class AttackType { Melee, Range };
			AttackType Type{ AttackType::Melee };

			float Damage{ 10.0f };
			bool Friendly{ 0 };			// A friendly attack effects neither the Player or the attack. 1 = friendly attack, 0 = enemy attack

			float Lifetime{ 0.5f };
			float LifetimeTimer{ Frosty::Time::CurrentTime() };

			bool Destroyable{ true };	// Cannot be destr0yed in collision with other enemies

			CAttack() = default;
			CAttack(AttackType type, float damage, bool friendly = 0, float lifeTime = 0.5f, bool destroyable = true) : Type(type), Damage(damage), Friendly(friendly), Lifetime(lifeTime), Destroyable(destroyable) { }
			CAttack(const CAttack& org) { FY_CORE_ASSERT(false, "Copy constructor in CAttack called."); }
			CAttack& operator=(const CAttack& org)
			{
				if (this != &org)
				{
					Type = org.Type;
					Damage = org.Damage;
					Friendly = org.Friendly;
					Destroyable = org.Destroyable;
					Lifetime = org.Lifetime;
					LifetimeTimer = org.LifetimeTimer;
				}

				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CPlayer : public BaseComponent
		{
			static std::string NAME;

			CWeapon* Weapon{ nullptr };

			int MoveLeftKey{ FY_KEY_A };
			int MoveForwardKey{ FY_KEY_W };
			int MoveRightKey{ FY_KEY_D };
			int MoveBackKey{ FY_KEY_S };
			int DashKey{ FY_KEY_LEFT_SHIFT };
			//temp for creation of level (I dont want arrows to spwan when i save a level)
			int LVL1Attack{ FY_MOUSE_BUTTON_RIGHT };
			int LVL2Attack{ FY_MOUSE_BUTTON_RIGHT };
			int LVL3Attack{ FY_KEY_SPACE };

			int HealingPotionKey{ FY_KEY_1 };
			int SpeedPotionKey{ FY_KEY_2 };
			int DropBaitKey{ FY_KEY_Q };

			CPlayer() = default;
			CPlayer(CWeapon* weapon) : Weapon(weapon) { }
			CPlayer(const CPlayer& org) { FY_CORE_ASSERT(false, "Copy constructor in CPlayer called."); }
			CPlayer& operator=(const CPlayer& org)
			{
				if (this != &org)
				{
					Weapon = org.Weapon;

					MoveLeftKey = org.MoveLeftKey;
					MoveForwardKey = org.MoveForwardKey;
					MoveRightKey = org.MoveRightKey;
					MoveBackKey = org.MoveBackKey;
					DashKey = org.DashKey;
					LVL1Attack = org.LVL1Attack;
					LVL2Attack = org.LVL2Attack;
					LVL3Attack = org.LVL3Attack;

					HealingPotionKey = org.HealingPotionKey;
					SpeedPotionKey = org.SpeedPotionKey;
					DropBaitKey = org.DropBaitKey;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CEnemy : public BaseComponent
		{
			static std::string NAME;

			CWeapon* Weapon{ nullptr };
			CTransform* Target{ nullptr };
			
			glm::vec3 CellTarget{ 0.0f };
			float AttackRange{ 2.5f };
			float SightRange{ 40.0f };

			CEnemy() = default;
			CEnemy(CWeapon* weapon) : Weapon(weapon) { }
			CEnemy(CTransform* target) : Target(target) { }
			CEnemy(const CEnemy& org) { FY_CORE_ASSERT(false, "Copy constructor in CEnemy called."); }
			CEnemy& operator=(const CEnemy& org)
			{
				if (this != &org)
				{
					Weapon = org.Weapon;
					Target = org.Target;

					CellTarget = org.CellTarget;
					AttackRange = org.AttackRange;
					SightRange = org.SightRange;
				}
				return *this;
			}
			virtual std::string GetName() const { return NAME; }
		};

		struct CHealth : public BaseComponent
		{
			static std::string NAME;
			float MaxPossibleHealth{ 20 };							// Max health an entity can upgrade to
			float MaxHealth{ 5 };								// Max health an entity can currently have
			float CurrentHealth{ 5 };

			CHealth() = default;
			CHealth(float health) : MaxHealth(health), CurrentHealth(health) {};
			CHealth(const CHealth& org) { FY_CORE_ASSERT(false, "Copy constructor in CHealth called."); }
			CHealth& operator=(const CHealth& org)
			{
				if (this != &org)
				{
					MaxPossibleHealth = org.MaxPossibleHealth;
					MaxHealth = org.MaxHealth;
					CurrentHealth = org.CurrentHealth;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CInventory : public BaseComponent
		{
			static std::string NAME;

			// HEALING POTION - heals consumer (temp)
			int MaxHealingPotions{ 5 };							// Max number of healing potions
			int CurrentHealingPotions{ 3 };						// Current number of potions in inventory
			float Heal{ 5.f };									// Heals 5 hearts
			float HealingCooldown{ 3.f };						// Consumer can only drink Healing Potion every 3rd second
			float HealingTimer{ Frosty::Time::CurrentTime() };			// Timer used to check cooldown

			// INCREASE HEALTH POTION - inreases max health on consumer (const)
			int MaxIncreaseHPPotions{ 5 };
			int CurrentIncreaseHPPotions{ 3 };
			float IncreaseHP{ 3.f };
			float IncreaseHPCooldown{ 3.f };
			float IncreaseHPTimer{ Frosty::Time::CurrentTime() };

			// SPEED BOOSTER POTION - boosts speed during a time interval (temp)
			int MaxSpeedPotions{ 5 };
			int CurrentSpeedPotions{ 3 };
			float IncreaseSpeedTemporary{ 0.2f };
			float SpeedCooldown{ 5.f };
			float SpeedTimer{ Frosty::Time::CurrentTime() };

			// SPEED BOOTS - boots add speed by a small procentage (const)
			int MaxSpeedBoots{ 5 };
			int CurrentSpeedBoots{ 0 };
			float IncreaseSpeed{ 1.f };

			// BAIT - chunks of meat used to distract the wolf
			int MaxBaitAmount{ 5 };
			int CurrentBaitAmount{ 100 };
			float BaitCooldown{ 1.f };
			float BaitTimer{ Frosty::Time::CurrentTime() };

			// WOLFSBANE - poisonous flower, used as currency
			int CurrentWolfsbane{ 0 };

			CInventory() = default;
			CInventory(const CInventory& org) { FY_CORE_ASSERT(false, "Copy constructor in CInventory called."); }
			CInventory& operator=(const CInventory& org)
			{
				if (this != &org)
				{
					MaxHealingPotions = org.MaxHealingPotions;
					CurrentHealingPotions = org.CurrentHealingPotions;
					Heal = org.Heal;
					HealingCooldown = org.HealingCooldown;
					HealingTimer = org.HealingTimer;

					MaxIncreaseHPPotions = org.MaxIncreaseHPPotions;
					CurrentIncreaseHPPotions = org.CurrentIncreaseHPPotions;
					IncreaseHP = org.IncreaseHP;
					IncreaseHPCooldown = org.IncreaseHPCooldown;
					IncreaseHPTimer = org.IncreaseHPTimer;

					MaxSpeedPotions = org.MaxSpeedPotions;
					CurrentSpeedPotions = org.CurrentSpeedPotions;
					IncreaseSpeedTemporary = org.IncreaseSpeedTemporary;
					SpeedCooldown = org.SpeedCooldown;
					SpeedTimer = org.SpeedTimer;

					MaxSpeedBoots = org.MaxSpeedBoots;
					CurrentSpeedBoots = org.CurrentSpeedBoots;
					IncreaseSpeed = org.IncreaseSpeed;

					MaxBaitAmount = org.MaxBaitAmount;
					CurrentBaitAmount = org.CurrentBaitAmount;
					BaitCooldown = org.BaitCooldown;
					BaitTimer = org.BaitTimer;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CHealthBar : public BaseComponent
		{
			static std::string NAME;
			glm::vec3 BarOffset{ 0.0f, 5.0f, 0.0f };
			std::shared_ptr<VertexArray> Mesh;
			std::shared_ptr<Shader> UseShader;
			std::shared_ptr<Texture2D> Texture;

			glm::vec3 Translate{ 1.0f };
			glm::vec3 Scale{ 1.0f };

			CHealthBar() = default;
			CHealthBar(glm::vec3 barOffset) : BarOffset(barOffset) { }
			CHealthBar(glm::vec3 barOffset, std::shared_ptr<VertexArray> mesh, std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> tex)
				: BarOffset(barOffset), Mesh(mesh), UseShader(shader), Texture(tex) { }
			CHealthBar(const CHealthBar& org) { FY_CORE_ASSERT(false, "Copy constructor in CHealthBar called."); }
			CHealthBar& operator=(const CHealthBar& org)
			{
				if (this != &org)
				{
					BarOffset = org.BarOffset;
					Mesh = org.Mesh;
					UseShader = org.UseShader;
					Texture = org.Texture;

					Translate = org.Translate;
					Scale = org.Scale;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CDash : public BaseComponent
		{
			static std::string NAME;
			static const int COOLDOWN = 3000;
			static const int DISTANCE = 16000;
			bool Active{ false };
			float CurrentCooldown{ 0.0f };
			float DistanceDashed{ 0.0f };
			float SpeedMultiplier{ 7.0f };

			CDash() = default;
			CDash(const CDash& org) { FY_CORE_ASSERT(false, "Copy constructor in CDash called."); }
			CDash& operator=(const CDash& org)
			{
				if (this != &org)
				{
					Active = org.Active;
					CurrentCooldown = org.CurrentCooldown;
					DistanceDashed = org.DistanceDashed;
					SpeedMultiplier = org.SpeedMultiplier;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CDestroy : public BaseComponent
		{
			static std::string NAME;

			CDestroy() = default;
			CDestroy(const CDestroy& org) { FY_CORE_ASSERT(false, "Copy constructor in CDestroy called."); }

			virtual std::string GetName() const { return NAME; }
		};

		struct CParticleSystem : public BaseComponent
		{
			static std::string NAME;
			
			struct Particle
			{
				glm::vec4 Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				glm::vec4 Color = glm::vec4(1.0f);
				float Size = 1.0f; //The current size

				glm::vec4 Direction = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				glm::vec4 StartPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				float Lifetime = -1.0f;
				float Speed = 2.0f;
				float StartSize = 1.0f;
				float EndSize = 1.0f;

				float CamDistance = -1.0f; //For sorting

				Particle() = default;
				Particle(glm::vec4 color)
					: Color(color) {};

				bool operator<(Particle& that) // Sort in reverse order, far particles drawn first
				{
					return this->CamDistance > that.CamDistance;
				}
			};

			struct GPUParticle
			{
				glm::vec4 Position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				glm::vec4 Color = glm::vec4(1.0f);
				float Size = 1.0f;
			};

			static const uint32_t MAX_PARTICLE_COUNT = 100;
			//uint32_t maxParticles = 0;
			float StartParticleSize = 1.0f;
			float EndParticleSize = 0.0f;
			float ParticleSize = 1.0f; //For a constant size

			uint32_t ParticleCount = 0;
			glm::vec3 ParticleSystemColor = glm::vec3(1.0f);
			float EmitRate = 0.1f;
			uint32_t EmitCount = 1;
			float MaxLifetime = 3.0f; //All particles
			float FadeTreshold = 0.0f; //No fade
			bool Preview = false;
			float Timer = 0.0f;

			uint32_t LastUsedParticle = 0;

			Particle Particles[MAX_PARTICLE_COUNT]; //The complete data
			GPUParticle GpuParticles[MAX_PARTICLE_COUNT]; //The data we send to the gpu

			std::shared_ptr<VertexArray> ParticleVertArray;
			std::shared_ptr<Shader> UseShader;
			std::shared_ptr<Texture2D> Texture;

			CParticleSystem() = default;
			CParticleSystem(std::shared_ptr<VertexArray> verts, std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> tex)
				: ParticleVertArray(verts), UseShader(shader), Texture(tex) {}
			CParticleSystem(std::shared_ptr<VertexArray> verts, std::shared_ptr<Shader> shader, std::shared_ptr<Texture2D> tex, glm::vec4 color = glm::vec4(1.0f), float particleSize = 1.0f)
				: ParticleVertArray(verts), UseShader(shader), Texture(tex), ParticleSystemColor(color), ParticleSize(particleSize)
			{
				for (uint32_t i = 0; i < MAX_PARTICLE_COUNT; i++)
				{
					Particles[i].Color = color;
					Particles[i].Size = particleSize;
				}
			}
			CParticleSystem(const CParticleSystem& org) { FY_CORE_ASSERT(false, "Copy constructor in CParticleSystem called."); }

			virtual std::string GetName() const { return NAME; }
		};

		struct CLootable : public BaseComponent
		{
			static std::string NAME;
			enum class LootType 
			{ 
				HealingPotion, IncHealthPotion, SpeedPotion, SpeedBoot, 
				Sword1, Sword2, Sword3,
				Arrow1, Arrow2, Arrow3
			};
			LootType Type{ LootType::HealingPotion };

			CLootable() = default;
			CLootable(LootType type) : Type(type) {}
			CLootable(const CLootable& org) { FY_CORE_ASSERT(false, "Copy constructor in CLootable called."); }
			CLootable& operator=(const CLootable& org) 
			{
				if (this != &org)
				{
					Type = org.Type;
				}
				return *this;
			}
			virtual std::string GetName() const { return NAME; }
		};

		struct CDropItem : public BaseComponent
		{
			static std::string NAME;

			CDropItem() = default;
			CDropItem(const CDropItem& org) { FY_CORE_ASSERT(false, "Copy constructor in CDropItem called."); }

			virtual std::string GetName() const { return NAME; }
		};

		struct CBoss : public BaseComponent
		{
			static std::string NAME;
			float DistractionTime{ 3.0f };
			float DistractionTimer{ Frosty::Time::CurrentTime() };
			bool Distracted{ false };
			bool Hunting{ false };
			std::vector<std::shared_ptr<Frosty::ECS::Entity>> TargetList;

			CBoss() = default;
			CBoss(float DistractionTime) : DistractionTime(DistractionTime) { }
			CBoss(const CBoss& org) { FY_CORE_ASSERT(false, "Copy constructor in CBoss called."); }
			CBoss& operator=(const CBoss& org)
			{
				if (this != &org)
				{
					DistractionTime = org.DistractionTime;
					DistractionTimer = org.DistractionTimer;
					Distracted = org.Distracted;
					Hunting = org.Hunting;

					TargetList = org.TargetList;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }

		};

		struct CLevelExit : public BaseComponent
		{
			static std::string NAME;
			//up = 0, down = 1, right = 2, left = 3
			int ExitDirection{ 0 };

			CLevelExit() = default;
			CLevelExit(int exitDirection) : ExitDirection(exitDirection) { }
			CLevelExit(const CLevelExit& org) { FY_CORE_ASSERT(false, "Copy constructor in CLevelExit called."); }
			CLevelExit& operator=(const CLevelExit& org)
			{
				if (this != &org)
				{
					ExitDirection = org.ExitDirection;
				}
				return *this;
			}

			virtual std::string GetName() const { return NAME; }
		};

		struct CGUI : public BaseComponent
		{
			static std::string NAME;

			std::shared_ptr<Shader> TextShader;
			std::shared_ptr<Shader> SpriteShader;

			UILayout Layout;

			CGUI() = default;
			CGUI(UILayout& layout) : Layout(layout) {  }
			CGUI(const CGUI& org) { FY_CORE_ASSERT(false, "Copy constructor in CGUI called."); }
			CGUI& operator=(const CGUI& org)
			{
				if (this != &org)
				{
					TextShader = org.TextShader;
					SpriteShader = org.SpriteShader;
					Layout = org.Layout;
				}
				return *this;
			}
			virtual std::string GetName() const { return NAME; }
		};

		static std::string GetComponentName(size_t i)
		{
			switch (i)
			{
				case 0:		return "Transform";
				case 1:		return "Mesh";
				case 2:		return "Camera";
				case 3:		return "Material";
				case 4:		return "Follow";
				case 5:		return "Light";
				case 6:		return "Physics";
				case 7:		return "Weapon";
				case 8:		return "Attack";
				case 9:		return "Player";
				case 10:	return "Enemy";
				case 11:	return "Health";
				case 12:	return "Inventory";
				case 13:	return "HealthBar";
				case 14:	return "Dash";
				case 15:	return "Destroy";
				case 16:	return "ParticleSystem";
				case 17:	return "Lootable";
				case 18:	return "DropItem";
				case 19:	return "Boss";
				case 20:	return "LevelExit";
				case 21:	return "GUI";
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
			virtual void OnStart() { }
			virtual void OnInput() { }
			virtual void OnUpdate() { }
			virtual void OnEvent(BaseEvent& e) { }
			virtual void BeginScene() { }
			virtual void Render() { }

			virtual void AddComponent(const std::shared_ptr<Entity>& entity) = 0;
			virtual void RemoveEntity(const std::shared_ptr<Entity>& entity) = 0;
			virtual void UpdateEntityComponent(const std::shared_ptr<Entity>& entity) = 0;
			virtual std::string GetInfo() const = 0;

			ComponentBitset& GetSignature() { return p_Signature; }

			std::map<std::shared_ptr<Entity>, ComponentArrayIndex>::iterator begin() { return p_EntityMap.begin(); }
			std::map<std::shared_ptr<Entity>, ComponentArrayIndex>::iterator end() { return p_EntityMap.end(); }

			friend std::ostream& operator<<(std::ostream& out, const BaseSystem& bs)
			{
				return out << bs.GetInfo();
			}

		protected:
			ComponentBitset p_Signature;
			std::map<std::shared_ptr<Entity>, ComponentArrayIndex> p_EntityMap;
			ComponentArrayIndex p_Total{ 1 };

		private:
		};

#pragma endregion System

	}
}

#endif // !ECS_HPP

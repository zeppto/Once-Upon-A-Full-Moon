#ifndef ABILITY_EVENT_HPP
#define ABILITY_EVENT_HPP

#include "EventSystem.hpp"
#include "Frosty/API/AssetManager/AssetFiles/BoolMap.hpp"

namespace Frosty
{
	class BoolMap; 
	namespace ECS { struct Entity; }

	class CollisionEvent : public BaseEvent
	{
	public:
		CollisionEvent(const std::shared_ptr<ECS::Entity>& entityA, const std::shared_ptr<ECS::Entity>& entityB)
			: m_EntityA(entityA), m_EntityB(entityB) { }

		const std::shared_ptr<ECS::Entity>& GetEntityA() const { return m_EntityA; }
		const std::shared_ptr<ECS::Entity>& GetEntityB() const { return m_EntityB; }
		std::shared_ptr<ECS::Entity>& GetEntityA() { return m_EntityA; }
		std::shared_ptr<ECS::Entity>& GetEntityB() { return m_EntityB; }
		const glm::vec3& GetOffset() const { return m_Offset; }

		EVENT_TYPE(Collision)

	private:
		std::shared_ptr<ECS::Entity> m_EntityA;
		std::shared_ptr<ECS::Entity> m_EntityB;
		glm::vec3 m_Offset{ 0.0f };
	};

	class BasicAttackEvent : public BaseEvent
	{
	public:
		BasicAttackEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(BasicAttack)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class DashEvent : public BaseEvent
	{
	public:
		DashEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(Dash)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class PickUpAttemptEvent : public BaseEvent
	{
	public:
		PickUpAttemptEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }
		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(PickUpAttempt)
	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class PickUpEvent : public BaseEvent
	{
	public:
		PickUpEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }
		std::shared_ptr<ECS::Entity>& GetEntity() { return m_Entity; }

		EVENT_TYPE(PickUp)
	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class ResetEvent : public BaseEvent
	{
	public:
		ResetEvent() { }

		EVENT_TYPE(Reset)
	private:
	};

	class BaitPlacedEvent : public BaseEvent
	{
	public:
		BaitPlacedEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(BaitPlaced)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class PlayAnimEvent : public BaseEvent
	{
	public:
		PlayAnimEvent(const std::shared_ptr<ECS::Entity>& entity, unsigned int animID) : m_Entity(entity), m_AnimID(animID) {}
		//Anim ID:
		// 0 is die anim
		// 1,2,3 is sword attacks
		// 4 is bow anim

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }
		const unsigned int * getAnimID() { return &m_AnimID; }

		EVENT_TYPE(PlayAnim);
	private:
		std::shared_ptr<ECS::Entity> m_Entity;
		unsigned int m_AnimID;
	};

	class ExitLevelEvent : public BaseEvent
	{
	public:
		ExitLevelEvent(const std::shared_ptr<ECS::Entity>& exit, const std::shared_ptr<ECS::Entity>& player) : m_ExitEntity(exit), m_PlayerEntity(player){ }

		const std::shared_ptr<ECS::Entity>& GetExitEntity() const { return m_ExitEntity; }
		const std::shared_ptr<ECS::Entity>& GetPlayerEntity() const { return m_PlayerEntity; }

		EVENT_TYPE(ExitCurrentLevel)

	private:
		std::shared_ptr<ECS::Entity> m_ExitEntity;
		std::shared_ptr<ECS::Entity> m_PlayerEntity;
	};

	class EnterNewRoomEvent : public BaseEvent
	{
	public:
		EnterNewRoomEvent(const int totalVisitedRooms) : m_TotalVisitedRooms(totalVisitedRooms) { }

		const int GetTotalvisitedRooms() const { return m_TotalVisitedRooms; }

		EVENT_TYPE(EnterNewRoom)

	private:
		int m_TotalVisitedRooms = 0;
	};

	class SaveLevelEvent : public BaseEvent
	{
	public:
		SaveLevelEvent() {}

		EVENT_TYPE(SaveLevel)

	private:
	};

	class BoolMapLoadedEvent : public BaseEvent
	{
	public:
		BoolMapLoadedEvent(const std::shared_ptr<BoolMap>& BoolMap) :  m_BoolMap(BoolMap){}

		inline const std::shared_ptr<BoolMap>& GetBoolMap() { return m_BoolMap; }

		EVENT_TYPE(LoadBoolMap)

	private:
		std::shared_ptr <BoolMap> m_BoolMap;
	};

	class CreateLevelEvent : public BaseEvent
	{
	public:
		CreateLevelEvent(const bool& up, const bool& down, const bool& left, const bool& right) : m_Up(up), m_Down(down), m_Left(left), m_Right(right){}

		//0 = Up, 1 = down, 2 = left, 3 = right
		const bool GetDirections(int direction) {
			if (direction == 0) return m_Up; if (direction == 1) return m_Down; 
			if (direction == 2) return m_Left; if (direction == 3) return m_Right;
			return false;
		}

		EVENT_TYPE(CreateLevel)

	private:
		bool m_Up;
		bool m_Down;
		bool m_Left;
		bool m_Right;
	};

	class OpenLevelEvent : public BaseEvent
	{
	public:
		OpenLevelEvent(std::string filename) : m_Filename(filename){}

		const std::string GetFilename() { return m_Filename; }

		EVENT_TYPE(OpenLevel)

	private:
		std::string m_Filename;
	};

	class CreatEntityEvent : public BaseEvent
	{
	public:
		CreatEntityEvent(unsigned int preSetEntityID) : m_EntityType(preSetEntityID) {}

		const unsigned int GetEntityType() { return m_EntityType; }

		EVENT_TYPE(CreatEntity)

	private:
		unsigned int m_EntityType;
	};


	class UpdatePlayerRoomCoordEvent : public BaseEvent
	{
	public:
		UpdatePlayerRoomCoordEvent(const glm::ivec2& Coords) : m_Coords(Coords) { }

		const glm::ivec2& GetCoords() const { return m_Coords; }


		EVENT_TYPE(UpdatePlayerCoordsPos)

	private:
		glm::ivec2 m_Coords;
	};



	class UpdateCurrentRoomEvent : public BaseEvent
	{
	public:
		UpdateCurrentRoomEvent(const std::string& CurrentRoom, const int& rotation) : m_CurrentRoom(CurrentRoom), m_Rotation(rotation){}

		inline const std::string& GetCurrentRoom() { return m_CurrentRoom; }
		inline const int& GetRotation() { return m_Rotation; }

		EVENT_TYPE(UpdateCurrentRoom)

	private:
		std::string m_CurrentRoom;
		int m_Rotation;
	};
	

		class SwitchRoomEvent : public BaseEvent
	{
	public:
		SwitchRoomEvent() {}
		EVENT_TYPE(SwitchRoom)

	private:

	};

	class InitiateGridEvent : public BaseEvent
	{
	public:
		InitiateGridEvent(Frosty::ECS::CTransform* planeTransform, const uint32_t& EntityGroup) : m_Transform(planeTransform), m_EntityGroup(EntityGroup){}

		Frosty::ECS::CTransform* GetTransform() { return m_Transform; }
		inline const uint32_t& GetEntityGroup() const noexcept { return m_EntityGroup; }

		EVENT_TYPE(InitiateGridMap)

	private:
		Frosty::ECS::CTransform* m_Transform;
		uint32_t m_EntityGroup;
	};

	class ActivateWitchCircleEvent : public BaseEvent
	{
	public:
		ActivateWitchCircleEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(ActivateWitchCircle)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};

	class UpgradeWeaponEvent : public BaseEvent
	{
	public:
		UpgradeWeaponEvent(){ }

		EVENT_TYPE(UpgradeWeapon)
	};

	class HealAbilityEvent : public BaseEvent
	{
	public:
		HealAbilityEvent() { }

		EVENT_TYPE(HealAbility)
	};

	class EnemyDeathEvent : public BaseEvent
	{
	public:
		EnemyDeathEvent(int points) : m_Points(points) { }

		int GetPoints() { return m_Points; }

		EVENT_TYPE(EnemyDeath)

	private:
		int m_Points{ 0 };
	};

	class PlayerDamageEvent : public BaseEvent
	{
	public:
		PlayerDamageEvent() { }

		EVENT_TYPE(PlayerDamage)
	};

	class DropItemEvent : public BaseEvent
	{
	public:
		DropItemEvent(const std::shared_ptr<ECS::Entity>& entity) : m_Entity(entity) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }

		EVENT_TYPE(DropItem)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
	};
	
	class DamageEvent : public BaseEvent
	{
	public:
		DamageEvent(const std::shared_ptr<ECS::Entity>& entity, float damage) : m_Entity(entity), m_Damage(damage) { }

		const std::shared_ptr<ECS::Entity>& GetEntity() const { return m_Entity; }
		float GetDamage() const { return m_Damage; }

		EVENT_TYPE(Damage)

	private:
		std::shared_ptr<ECS::Entity> m_Entity;
		float m_Damage;
	};

	class GameoverEvent : public BaseEvent
	{
	public:
		GameoverEvent() { }

		EVENT_TYPE(GameOver)
	};

	class WinEvent : public BaseEvent
	{
	public:
		WinEvent() { }

		EVENT_TYPE(Win)
	};

	class BossSpawnedEvent : public BaseEvent
	{
	public:
		BossSpawnedEvent() { }

		EVENT_TYPE(BossSpawned)
	};

	class ResetBossAbilitiesEvent : public BaseEvent
	{
	public:
		ResetBossAbilitiesEvent(const std::shared_ptr<ECS::Entity>& boss) : m_Boss(boss) { }

		const std::shared_ptr<ECS::Entity>& GetBoss() const { return m_Boss; }

		EVENT_TYPE(ResetBoss)

	private:
		std::shared_ptr<ECS::Entity> m_Boss;

	};

	class SpawnBossEvent : public BaseEvent
	{
	public:
		SpawnBossEvent() { }


		EVENT_TYPE(SpawnBoss)

	private:

	};

	class BossFearEffectEvent : public BaseEvent
	{
	public:
		BossFearEffectEvent(glm::ivec2 bossDirection) : m_Direction(bossDirection) {}

		glm::ivec2 GetDirectionToBoss() { return m_Direction; }

		EVENT_TYPE(BossFearEffect)

	private:
		glm::ivec2 m_Direction;
	};
}
#endif // !ABILITY_EVENT_HPP

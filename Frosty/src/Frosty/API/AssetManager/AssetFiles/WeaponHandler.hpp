#ifndef WEAPONHANDLER_HPP
#define WEAPONHANDLER_HPP

namespace Frosty
{
	struct Weapon
	{
		enum class WeaponType { Sword, Bow, Fangs};
		
		inline Weapon() {}
		inline Weapon(
			WeaponType type,
			std::string speciality,
			unsigned int level,
			float attackRange,
			float damage,
			float criticalHit,
			float criticalHitChance,
			float lvl1AttackCooldown, 
			float lvl2AttackCooldown, 
			float lvl3AttackCooldown,
			float lifetime)
		: Type(type), Speciality(speciality), Level(level), AttackRange(attackRange), Damage(damage), CriticalHit(criticalHit), CriticalHitChance(criticalHitChance), LVL1AttackCooldown(lvl1AttackCooldown), LVL2AttackCooldown(lvl2AttackCooldown), LVL3AttackCooldown(lvl3AttackCooldown), Lifetime(lifetime) {}
		inline virtual ~Weapon() {}

		
		WeaponType Type{ WeaponType::Sword };
		std::string Speciality{ "" };
		uint8_t Level{ 1 };
		float AttackRange{ 0.f };
		float Damage{ 0.f };
		float CriticalHit{ 0.f };
		float CriticalHitChance{ 0.f };
		float LVL1AttackCooldown{ 0.f };
		float LVL2AttackCooldown{ 0.f };
		float LVL3AttackCooldown{ 0.f };
		float Lifetime{ 0.f };
	};


	class WeaponHandler
	{
	public:
		WeaponHandler();
		virtual ~WeaponHandler() {};
		WeaponHandler(const WeaponHandler& org) { FY_CORE_ASSERT(false, "Copy Constructor in WeaponHandler called."); };
		WeaponHandler& operator=(const WeaponHandler& org) { FY_CORE_ASSERT(false, "Assignment Operator in WeaponHandler called."); return *this; };

		bool LoadWeapons(const std::string& filePath);
	
		const int GetNumberOfWeapons();
		const Weapon& GetWeaponAt(unsigned int index);
		const std::vector<Weapon>& GetAllWeapons();
		
		// Returns a random weapon depending on the maximim level required
		const Weapon& GetWeaponUpToLevel(unsigned int level);
		
		// Returns a random weapon depending on the minimum level required
		const Weapon& GetWeaponFromLevel(unsigned int level);
	
		// Returns a random weapon depending on the type required
		const Weapon& GetWeaponByType(Weapon::WeaponType type);

	private:
		std::vector<Weapon> m_Weapons;
		unsigned int m_LowestWeaponLevel;
		unsigned int m_HighestWeaponLevel;
	};
}
#endif // !WEAPONHANDLER_HPP
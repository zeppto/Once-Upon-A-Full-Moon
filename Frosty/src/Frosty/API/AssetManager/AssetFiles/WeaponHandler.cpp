#include <fypch.hpp>
#include "WeaponHandler.hpp"
#include <PugiXML/pugixml.hpp>

namespace Frosty
{
	WeaponHandler::WeaponHandler()
	{
		m_LowestWeaponLevel = 1;
		m_HighestWeaponLevel = 1;
	}

	bool WeaponHandler::LoadWeapons(const std::string& filePath)
	{
		// Create neccessary files (XML)
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(filePath.c_str());

		FY_CORE_ASSERT(result, "Error loading file {0}: {1}", "Weapons.xml", result.description());

	
		// Get all nodes
		pugi::xml_node weapons = doc.child("weapons");

		// Retrieve and save info
		size_t amount = std::distance(weapons.children().begin(), weapons.children().end());
		m_Weapons.reserve(amount);

		Weapon weapon;
		std::string type = "";
		for (pugi::xml_node w = weapons.child("weapon"); w; w = w.next_sibling("weapon"))
		{
			type = w.attribute("type").as_string();
			if (type == "sword")
				weapon.Type = Weapon::WeaponType::Sword;
			else if (type == "bow")
				weapon.Type = Weapon::WeaponType::Bow;
			else if (type == "fangs")
				weapon.Type = Weapon::WeaponType::Bite;

			weapon.Speciality = w.attribute("speciality").as_string();
			weapon.Level = w.attribute("level").as_uint();
			weapon.AttackRange = w.attribute("attackRange").as_float();
			weapon.Damage = w.attribute("damage").as_float();
			weapon.CriticalHit = w.attribute("criticalHit").as_float();
			weapon.CriticalHitChance = w.attribute("criticalHitChance").as_float();
			weapon.LVL1AttackCooldown = w.attribute("lvl1AttackCooldown").as_float();
			weapon.LVL2AttackCooldown = w.attribute("lvl2AttackCooldown").as_float();
			weapon.LVL3AttackCooldown = w.attribute("lvl3AttackCooldown").as_float();
			weapon.Lifetime = w.attribute("lifetime").as_float();

			m_Weapons.emplace_back(weapon.Type, weapon.Speciality, weapon.Level, weapon.AttackRange, weapon.Damage, weapon.CriticalHit, weapon.CriticalHitChance, weapon.LVL1AttackCooldown, weapon.LVL2AttackCooldown, weapon.LVL3AttackCooldown, weapon.Lifetime);
		
			unsigned int level = w.attribute("level").as_uint();
			if (level > m_HighestWeaponLevel)
				m_HighestWeaponLevel = level;
		}
		return true;
	}
	
	const int WeaponHandler::GetNumberOfWeapons()
	{
		return int(m_Weapons.size());
	}
	
	const Weapon& WeaponHandler::GetWeaponAt(unsigned int index)
	{
		FY_CORE_ASSERT(index >= int(m_Weapons.size()), "WeaponHandler: Invalid index in GetWeaponAt().");

		return m_Weapons[index];
	}

	const std::vector<Weapon>& WeaponHandler::GetAllWeapons()
	{
		return m_Weapons;
	}

	const Weapon& WeaponHandler::GetWeaponUpToLevel(unsigned int level)
	{
		FY_CORE_ASSERT(level >= m_LowestWeaponLevel, "WeaponHandler: Required level value in GetWeaponUpToLevel() is too low.");

		if (level >= m_HighestWeaponLevel)
		{
			level = m_HighestWeaponLevel;

			// Randomize between all weapons
			int randomIndex = rand() % int(m_Weapons.size());
			return m_Weapons[randomIndex];
		}
		else
		{
			std::vector<size_t> indices;
			indices.reserve(m_Weapons.size());

			for (size_t i = 0; i < m_Weapons.size(); i++)
			{
				if (m_Weapons[i].Level >= m_LowestWeaponLevel && m_Weapons[i].Level <= level)
					indices.emplace_back(i);
			}

			// Randomize between lowest level - max level required
			int randomIndex = rand() % int(indices.size());
			return m_Weapons[indices[randomIndex]];
		}
	}

	const Weapon& WeaponHandler::GetWeaponFromLevel(unsigned int level)
	{
		FY_CORE_ASSERT(level <= m_HighestWeaponLevel, "WeaponHandler: Required level value in GetWeaponFromLevel() is too high.");
		
		if (level <= m_LowestWeaponLevel)
		{
			level = m_LowestWeaponLevel;

			// Randomize between all weapons
			int randomIndex = rand() % int(m_Weapons.size());
			return m_Weapons[randomIndex];
		}
		else
		{
			std::vector<size_t> indices;
			indices.reserve(m_Weapons.size());

			for (size_t i = 0; i < m_Weapons.size(); i++)
			{
				if (m_Weapons[i].Level >= level && m_Weapons[i].Level <= m_HighestWeaponLevel)
					indices.emplace_back(i);
			}

			// Randomize between lowest level - max level required
			int randomIndex = rand() % int(indices.size());
			return m_Weapons[indices[randomIndex]];
		}
	}

	const Weapon& WeaponHandler::GetWeaponByType(Weapon::WeaponType type)
	{
		std::vector<size_t> indices;
		indices.reserve(m_Weapons.size());

		for (size_t i = 0; i < m_Weapons.size(); i++)
		{
			if (m_Weapons[i].Type == type)
				indices.emplace_back(i);
		}

		FY_CORE_ASSERT(int(indices.size()) > 0, "WeaponHandler: Weapon type in GetWeaponByType() hasn't been loaded and therefore cannot be fetched.");

		// Randomize between all weapons with the right type required
		int randomIndex = rand() % int(indices.size());
		return m_Weapons[indices[randomIndex]];
	}
}
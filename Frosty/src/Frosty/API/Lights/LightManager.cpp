#include "fypch.hpp"
#include "LightManager.hpp"

namespace Frosty
{
	LightManager* LightManager::s_Instance = nullptr;

	LightManager::LightManager()
	{
		s_Instance = this;

		// ONLY for testing purposes	~ W-_-W ~
		//m_Pointlights.emplace_back(std::make_shared<PointLight>(glm::vec3(2.f, 2.f, 4.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f, 2.f));
		//m_Pointlights.emplace_back(std::make_shared<PointLight>(glm::vec3(3.f, 3.f, -10.f), glm::vec4(1.f, 1.f, 1.f, 1.f), 1.f, 2.f));
	}

	LightManager::LightManager(const LightManager& other)
	{
		FY_CORE_ASSERT(false, "Copy constructor is used in class 'LightManager'.");
	}

	LightManager& LightManager::operator=(const LightManager& other)
	{
		FY_CORE_ASSERT(false, "Assignment operator is used in class 'LightManager'.");
		return *this;
	}

	void LightManager::AddPointLight(glm::vec3 position, glm::vec4 color, float strength, float radius, glm::vec2 linear_Quadratic)
	{
		m_Pointlights.emplace_back(std::make_shared<PointLight>(position, color, strength, radius, linear_Quadratic));
	}

	void LightManager::AddDirectionalLight(glm::vec4 color, float strength, glm::vec3 direction)
	{
		m_DirLights.emplace_back(std::make_shared<DirectionalLight>(color, strength, direction));
	}

	bool LightManager::RemovePointLightAt(uint16_t position)
	{
		bool removed = false;
		if (m_Pointlights.size() < size_t(position))
		{
			m_Pointlights.erase(m_Pointlights.begin() + position);
			removed = true;
		}
		return removed;
	}

	bool LightManager::RemoveDirectionalLightAt(uint16_t position)
	{
		bool removed = false;
		if (m_DirLights.size() < size_t(position))
		{
			m_DirLights.erase(m_DirLights.begin() + position);
			removed = true;
		}
		return removed;
	}

	void LightManager::ClearPointLights()
	{
		m_Pointlights.clear();
	}

	void LightManager::ClearDirectionalLights()
	{
		m_DirLights.clear();
	}

	void LightManager::ClearAllLights()
	{
		m_Pointlights.clear();
		m_DirLights.clear();
	}

	std::shared_ptr<PointLight>& LightManager::GetPointLightAt(uint16_t position)
	{
		return m_Pointlights.at(size_t(position));
	}

	const std::vector<std::shared_ptr<PointLight>>& LightManager::GetAllPointLights()
	{
		return m_Pointlights;
	}

	const int LightManager::GetNrOfPointLights()
	{
		return int(m_Pointlights.size());
	}

	std::shared_ptr<DirectionalLight>& LightManager::GetDirectionalLightAt(uint16_t position)
	{
		return m_DirLights.at(size_t(position));
	}

	const std::vector<std::shared_ptr<DirectionalLight>>& LightManager::GetAllDirectionalLights()
	{
		return m_DirLights;
	}

	const int LightManager::GetNrOfDirectionalLights()
	{
		return int(m_DirLights.size());
	}
}
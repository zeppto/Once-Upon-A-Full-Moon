#ifndef LIGHTMANAGER_HPP
#define LIGHTMANAGER_HPP

#include "PointLight.hpp"
#include "DirectionalLight.hpp"

namespace Frosty
{
	enum LightType
	{
		POINTLIGHT,
		DIRLIGHT
	};

	class LightManager
	{
	public:
		LightManager();
		virtual ~LightManager() {};
		LightManager(const LightManager& other);
		LightManager& operator=(const LightManager& other);

		// Add
		void AddPointLight(glm::vec3 position, glm::vec4 color, float strength, float radius, glm::vec2 linear_Quadratic = { 0.35f, 0.44f });
		void AddDirectionalLight(glm::vec4 color, float strength, glm::vec3 direction = { 0.f, -1.f, 0.f });

		// Remove
		bool RemovePointLightAt(uint16_t position);
		bool RemoveDirectionalLightAt(uint16_t position);
		void ClearPointLights();
		void ClearDirectionalLights();
		void ClearAllLights();

		// Get Lights
		std::shared_ptr<PointLight>& GetPointLightAt(uint16_t position);
		const std::vector< std::shared_ptr<PointLight>>& GetAllPointLights();	// Should it be const??		~ W-_-W ~
		const int GetNrOfPointLights();

		std::shared_ptr<DirectionalLight>& GetDirectionalLightAt(uint16_t position);
		const std::vector<std::shared_ptr<DirectionalLight>>& GetAllDirectionalLights();
		const int GetNrOfDirectionalLights();

	private:
		std::vector<std::shared_ptr<PointLight>> m_Pointlights;
		std::vector<std::shared_ptr<DirectionalLight>> m_DirLights;

		static LightManager* s_Instance;
	};
}
#endif
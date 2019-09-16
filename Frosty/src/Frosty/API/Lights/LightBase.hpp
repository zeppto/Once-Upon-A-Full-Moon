#ifndef LIGHTBASE_H
#define LIGHTBASE

#include <glm/glm.hpp>

namespace Frosty 
{
	class LightBase
	{
	public:
		static uint16_t s_LightIDCounter;
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Color;
		float m_Strength;
		uint8_t m_ID;

	public:
		LightBase();
		LightBase(glm::vec3 position, glm::vec3 color, float strength);
		virtual ~LightBase() {};
		LightBase(const LightBase& other);
		LightBase& operator=(const LightBase& other);
		bool operator==(const LightBase& other);

		// Set
		void SetPosition(glm::vec3 position);
		void SetColor(glm::vec3 color);
		void SetStrength(float strength);

		// Get
		const glm::vec3 GetPosition();
		const glm::vec3 GetColor();
		const float GetStrength();
		const uint8_t GetID();
	};
}
#endif
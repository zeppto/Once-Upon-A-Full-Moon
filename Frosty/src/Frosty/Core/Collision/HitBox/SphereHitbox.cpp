#include "fypch.hpp"
#include "SphereHitbox.hpp"
#include "glm/glm.hpp"

namespace Frosty
{

	Frosty::SphereHitbox::SphereHitbox()
	{

		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_Length = 1.0f;
		m_Raduis = 1.0f;

	}

	Frosty::SphereHitbox::SphereHitbox(const float& Length, const float& Raduis)
	{


		m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		m_Direction = glm::vec3(0.0f, 1.0f, 0.0f);

		m_Length = Length;
		m_Raduis = Raduis;

	}

	Frosty::SphereHitbox::~SphereHitbox()
	{
	}

	bool Frosty::SphereHitbox::IsCollidingWith(const SphereHitbox& other) const
	{
		float thisLength = ((m_Length / 2) + m_Raduis);
		float otherLength = ((other.m_Length / 2) + other.m_Raduis);
		glm::vec3 TempVec = other.m_Position - m_Position;
		bool returnValue = false;

		if (TempVec.length() < (thisLength + otherLength))
		{
			glm::vec3 OrthoVec = glm::cross(m_Direction, other.m_Direction);
			glm::vec3 Final = ((glm::dot(TempVec, OrthoVec) / glm::dot(OrthoVec, OrthoVec)) * OrthoVec);

			if (Final.length() < (m_Raduis + other.m_Raduis))
			{





			}



		}



		return returnValue;
	}
}

#include "fypch.hpp"
#include "SphereHitbox.hpp"
#include "glm/glm.hpp" 

namespace Frosty
{


	Frosty::SphereHitbox::SphereHitbox()
	{
	}

	Frosty::SphereHitbox::~SphereHitbox()
	{
	}

	bool Frosty::SphereHitbox::IsCollidingWith(glm::vec3 length, glm::vec3 center, glm::vec3 rotation, glm::vec3 otherLength, glm::vec3 otherCenter, glm::vec3 otherRotation)
	{
		int thisLongestSide = 1;
		glm::vec4 thisDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		
		int otherLongestSide = 1;
		glm::vec4 otherDirection = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		for (int i = 0; i < 3; i++)
		{
			if (length[i] > length[thisLongestSide])
				thisLongestSide = i;
			if (otherLength[i] > otherLength[otherLongestSide])
				otherLongestSide = i;
		}

		thisDirection[thisLongestSide] = 1.0f;
		otherDirection[otherLongestSide] = 1.0f;

		glm::vec3 this_Position = center;

		glm::mat4 rotationMatrix(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 finalRotation = rotationMatrix * thisDirection;
		glm::vec3 this_Direction = glm::normalize(finalRotation);

		float this_Raduis = (length[0] + length[1] + length[2] - length[thisLongestSide]) / 2;
		float this_Length = (length[thisLongestSide] -this_Raduis) * 2;

		glm::vec3 other_Position = otherCenter;

		rotationMatrix = glm::mat4(1.0f);
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(otherRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(otherRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotationMatrix = glm::rotate(rotationMatrix, glm::radians(otherRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		finalRotation = rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		glm::vec3 other_Direction = glm::normalize(finalRotation);

		float other_Raduis = (otherLength[0] + otherLength[1] + otherLength[2] - otherLength[otherLongestSide]) / 2;
		float other_Length = (otherLength[otherLongestSide] -other_Raduis) * 2;

		float t_Length = this_Length + this_Raduis;
		float o_Length = other_Length + other_Raduis;
		glm::vec3 TempPOVec = other_Position - this_Position;
		glm::vec3 This_Dir = this_Direction;
		glm::vec3 Other_Dir = other_Direction;
		bool returnValue = false;

		if (TempPOVec.length() <= (t_Length + o_Length))
		{



			glm::vec3 CylEdge_Vec = This_Dir * this_Length;
			glm::vec3 CylEdge_Pos_One = (this_Position + CylEdge_Vec);
			glm::vec3 CylEdge_Pos_Two = (this_Position - CylEdge_Vec);


			glm::vec3 OtherCylEdge_Vec = Other_Dir * other_Length;
			glm::vec3 OtherCylEdge_Pos_One = other_Position + OtherCylEdge_Vec;
			glm::vec3 OtherCylEdge_Pos_Two = other_Position - OtherCylEdge_Vec;

			if (calcShortestDist(CylEdge_Pos_One, CylEdge_Pos_Two, OtherCylEdge_Pos_One, OtherCylEdge_Pos_Two) <= (other_Raduis + this_Raduis))
			{
				returnValue = true;
			}

		}
		return returnValue;
	}

	float SphereHitbox::calcShortestDist(const glm::vec3& S1P0, const glm::vec3& S1P1, const glm::vec3& S2P0, const glm::vec3& S2P1)
	{
		float SMALL_NUM = 10e-8f;
		glm::vec3 u = S1P1 - S1P0;
		glm::vec3 v = S2P1 - S2P0;
		glm::vec3 w = S1P0 - S2P0;

		float a = glm::dot(u,u);
		float b = glm::dot(u,v);
		float c = glm::dot(v,v);
		float d = glm::dot(u,w);
		float e = glm::dot(v,w);

		float D = a * c - b * b;
		float sc, sN, sD = D;
		float tc, tN, tD = D;

		if (D < SMALL_NUM)
		{
			sN = 0.0f;
			sD = 1.0f;
			tN = e;
			tD = c;
		}
		else
		{
			sN = (b * e - c * d);
			tN = (a * e - b * d);
			if (sN < 0.0f)
			{
				sN = 0.0f;
				tN = e;
				tD = c;
			}
			else if (sN > sD)
			{
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if (tN < 0.0f)
		{
			tN = 0.0f;
			if (-d < 0.0f)
			{
				sN = 0.0f;
			}
			else if (-d > a)
			{
				sN = sD;
			}
			else
			{
				sN = -d;
				sD = a;
			}
		}
		else if (tN > tD)
		{
			tN = tD;
			if ((-d + b) < 0.0f)
			{
				sN = 0.0f;
			}
			else if ((-d + b) > a)
			{
				sN = sD;
			}
			else
			{
				sN = (-d + b);
				sD = a;
			}
		}

		sc = (std::abs(sN) < SMALL_NUM ? 0.0f : sN / sD);
		tc = (std::abs(tN) < SMALL_NUM ? 0.0f : tN / tD);
		return glm::length((w + (sc * u) - (tc * v)));
	}

}

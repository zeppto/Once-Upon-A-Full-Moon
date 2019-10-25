#include <fypch.hpp>
#include "CollisionDetection.hpp"

#include <glm/glm.hpp>

Frosty::CollisionDetection::CollisionDetection()
{
}

Frosty::CollisionDetection::~CollisionDetection()
{
}

bool Frosty::CollisionDetection::AABBIntersectMinMax(glm::vec3 min, glm::vec3 max, glm::vec3 otherMin, glm::vec3 otherMax)
{
	bool intersect = false;
	if (otherMin.x <= max.x && otherMax.x >= min.x &&
		otherMin.y <= max.y && otherMax.y >= min.y &&
		otherMin.z <= max.z && otherMax.z >= min.z)
	{
		intersect = true;
	}
	return intersect;
}

bool Frosty::CollisionDetection::AABBIntersect(glm::vec3 length, glm::vec3 center, glm::vec3 otherLength, glm::vec3 otherCenter)
{
	bool intersect = false;
	glm::vec3 min = center - length, max = center + length,
		otherMin = otherCenter - otherLength, otherMax = otherCenter + otherLength;
	if (otherMin.x <= max.x && otherMax.x >= min.x &&
		otherMin.y <= max.y && otherMax.y >= min.y &&
		otherMin.z <= max.z && otherMax.z >= min.z)
	{
		intersect = true;
	}
	return intersect;
}

glm::vec3 Frosty::CollisionDetection::AABBIntersecPushback(glm::vec3 pushbackLength, glm::vec3 pushbackCenter, glm::vec3 otherLength, glm::vec3 otherCenter)
{
	glm::vec3 toPush(0.0f), toReturn(0.0f), pbMin = pushbackCenter - pushbackLength, pbMax = pushbackCenter + pushbackLength,
		otherMin = otherCenter - otherLength, otherMax = otherCenter + otherLength;
	glm::vec3 toPush1(0.0f),toPush2(0.0f);
	float smallestPush = 0;
	bool isZero = false;
	if (AABBIntersect(pushbackLength, pushbackCenter, otherLength, otherCenter))
	{
		for (int i = 0; i < 3; i++)
		{
			if (pbMax[i] >= otherMin[i] && pbMax[i] < otherMax[i])
			{
				toPush[i] = pbMax[i] - otherMin[i];
				if (abs(smallestPush) > abs(toPush[i]) || !isZero && smallestPush == 0)
					smallestPush = toPush[i];
				if (toPush[i] == 0)
					isZero = true;
			}
			if (otherMax[i] >= pbMin[i] && otherMin[i] < pbMin[i])
			{
				toPush[i] = pbMin[i] - otherMax[i];
				if (abs(smallestPush) > abs(toPush[i]) || !isZero && smallestPush == 0)
					smallestPush = toPush[i];
				if (toPush[i] == 0)
					isZero = true;
			}
			toPush1[i] = pbMax[i] - otherMin[i];
			toPush2[i] = pbMin[i] - otherMax[i];
		}
		for (int i = 0; i < 3; i++)
		{
			if (smallestPush == toPush[i])
			{
				if(smallestPush < -1)
					FY_TRACE("puch back ({0}) in ({1})", smallestPush, i);
				FY_TRACE("puch back ({0}) in ({1})", smallestPush, i);
				toReturn[i] = smallestPush;
				break;
			}
		}
	}
	return toReturn;
}

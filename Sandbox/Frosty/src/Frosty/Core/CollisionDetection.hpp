#ifndef COLLISIONDETECTION_HPP
#define COLLISIONDETECTION_HPP

namespace Frosty
{
	class CollisionDetection
	{
	public:
		CollisionDetection();
		~CollisionDetection();

		//AABBIntersect the hitbox minimum valu and maximum (first one then the other hitbox)
		bool AABBIntersectMinMax( glm::vec3 min, glm::vec3 max, glm::vec3 otherMin, glm::vec3 otherMax);
		//AABBIntersect the hitboxs length from center and the center (first one then the other hitbox)
		static bool AABBIntersect(glm::vec3 length, glm::vec3 center, glm::vec3 otherLength, glm::vec3 otherCenter);

		static glm::vec3 AABBIntersecPushback(glm::vec3 pushbackLength, glm::vec3 pushbackCenter, glm::vec3 otherLength, glm::vec3 otherCenter);

		static bool AABBRayIntersect(glm::vec3 rayStart, glm::vec3 rayLength, glm::vec3 AABBLength, glm::vec3 AABBCenter);


	private:
		 

	};
}

#endif

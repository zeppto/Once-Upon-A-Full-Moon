#ifndef ANIMATIONHANDLER_H
#define ANIMATIONHANDLER_H
#include <Luna/include/Luna.h>
#include <vector>
#include <map>
#include <Windows.h>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Frosty
{
	class AnimationHandler
	{
	private:
		Luna::Animation * animPtr;
		std::vector<Luna::Joint> jointVec;
		std::map<uint16_t, std::vector<Luna::Keyframe>> keyFrameMap;

		//Temp. This should either be an input or a dynamically allocated array.
		glm::mat4 skinData[64];

	public:
		AnimationHandler();
		AnimationHandler(Luna::Animation * Aptr, std::vector<Luna::Joint> jVec, std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap);
		~AnimationHandler();

		void setAnimPtr(Luna::Animation * Aptr);
		void setJointVec(std::vector<Luna::Joint> jVec );
		void setKeyframeMap(std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap);

		glm::mat4 * getSkinData();
		int getNrOfJoints();

		void CalculateAnimMatrix(float * currentAnimTime);

	private:
	};
}

#endif // !ANIMATIONHANDLER_H
#ifndef ANIMATIONHANDLER_H
#define ANIMATIONHANDLER_H
#include <Luna/include/Luna.h>
#include <vector>
#include <map>
#include <Windows.h>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define MAX_BONES 64

namespace Frosty
{

	struct JointTransforms
	{
		glm::mat4 jTrans[MAX_BONES];
	};

	class AnimationHandler
	{
	private:
		Luna::Animation * animPtr;
		std::vector<Luna::Joint> jointVec;
		std::map<uint16_t, std::vector<Luna::Keyframe>> keyFrameMap;

		JointTransforms skinData;
		//Temp. This should either be an input or a dynamically allocated array.

	public:
		AnimationHandler();
		AnimationHandler(Luna::Animation * Aptr, std::vector<Luna::Joint> jVec, std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap);
		~AnimationHandler();

		void setAnimPtr(Luna::Animation * Aptr);
		void setJointVec(std::vector<Luna::Joint> jVec );
		void setKeyframeMap(std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap);

		void getSkinData(void * &data);

		int getNrOfJoints();

		void CalculateAnimMatrix(float * currentAnimTime);

	private:
		void prepRotation(float arr[4]);
	};
}

#endif // !ANIMATIONHANDLER_H
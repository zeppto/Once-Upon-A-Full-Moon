#include "fypch.hpp"
#include "AnimationHandler.h"

Frosty::AnimationHandler::AnimationHandler()
{
}

Frosty::AnimationHandler::AnimationHandler(Luna::Animation * Aptr, std::vector<Luna::Joint> jVec, std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap)
{

}

Frosty::AnimationHandler::~AnimationHandler()
{
}

void Frosty::AnimationHandler::setAnimPtr(Luna::Animation * Aptr)
{
	animPtr = Aptr;
}

void Frosty::AnimationHandler::setJointVec(std::vector<Luna::Joint> jVec)
{
	jointVec = jVec;
}

void Frosty::AnimationHandler::setKeyframeMap(std::map<uint16_t, std::vector<Luna::Keyframe>>* kMap)
{
	keyFrameMap = kMap;
}

glm::mat4 * Frosty::AnimationHandler::getSkinData()
{
	return skinData;
}

int Frosty::AnimationHandler::getNrOfJoints()
{
	return jointVec.size();
}

#define MAXBONES 80

void Frosty::AnimationHandler::CalculateAnimMatrix( float currentAnimTime)
{
	if (currentAnimTime > animPtr->duration)
	{
		//TODO: Loop to beginning
	}
	//Since all animations are baked which joint it is irrelevant for determining last keyframe.
	std::vector<Luna::Keyframe> keyVec;
	keyVec = keyFrameMap->at(jointVec[0].jointID);

	// keyframes involved.
	int k1 = (int)(currentAnimTime * animPtr->fps);
	//keyFrameMap.
	int k2 = fminf(k1 + 1, keyVec[keyVec.size()].timePosition);

	// keyframes in anim_time terms
	float k1_time = k1 / animPtr->fps;
	float k2_time = k2 / animPtr->fps;
	// time rescaled into [0..1] as a percentage between k1 and k2
	float t = (currentAnimTime - k1_time) / (k2_time - k1_time);

	int boneCount = jointVec.size();

	glm::mat4 bone_global_pose[MAXBONES]{ glm::mat4(1.0) };
	//Initialize bones.
	for (int i = 0; i < MAXBONES; i++)
	{
		bone_global_pose[i] = glm::mat4(1.0);
	}
	keyVec = keyFrameMap->at(jointVec[0].jointID);

	///*glm::vec4 k2Trans = { keyVec[k2].translation[0] ,keyVec[k2].translation[1] ,keyVec[k2].translation[2] ,keyVec[k2].translation[3] };*/

	//TODO: All the make_ calls should be done before this function.
	//Ideally keyVec[k1].translation and the others would already be parsed correctly.
	glm::vec4 k1Trans = glm::make_vec4(keyVec[k1].translation);
	glm::vec4 k2Trans = glm::make_vec4(keyVec[k2].translation);

	glm::vec4 k1Scale = glm::make_vec4(keyVec[k1].scale);
	glm::vec4 k2Scale = glm::make_vec4(keyVec[k2].scale);

	glm::quat k1Rot = glm::make_quat(keyVec[k1].rotation);
	glm::quat k2Rot = glm::make_quat(keyVec[k2].rotation);

	////Root Values.
	glm::vec4 translation_r = k1Trans * (1 - t) + k2Trans * t;
	glm::vec4 scaling_r		= k1Scale * (1 - t) + k2Scale * t;
	glm::quat quaternion_r  = glm::slerp(k1Rot, k2Rot, t);
	// Transfer to matricies
	glm::mat4 idMat = glm::mat4(1.0f);
	glm::mat4 transMat = glm::translate(idMat, glm::vec3(translation_r));
	glm::mat4 scaleMat = glm::scale(idMat, glm::vec3(scaling_r));
	glm::mat4 rotMat = glm::mat4_cast(quaternion_r);
	// Put it in the list.
	bone_global_pose[0] = transMat * rotMat * scaleMat;

	////Apply final rotation to the array for root node.
	glm::mat4 invBindPose = { jointVec[0].invBindposeMatrix[0][0],jointVec[0].invBindposeMatrix[0][1],jointVec[0].invBindposeMatrix[0][2], jointVec[0].invBindposeMatrix[0][3],
							  jointVec[0].invBindposeMatrix[1][0],jointVec[0].invBindposeMatrix[1][1],jointVec[0].invBindposeMatrix[1][2], jointVec[0].invBindposeMatrix[1][3],
							  jointVec[0].invBindposeMatrix[2][0],jointVec[0].invBindposeMatrix[2][1],jointVec[0].invBindposeMatrix[2][2], jointVec[0].invBindposeMatrix[2][3],
							  jointVec[0].invBindposeMatrix[3][0],jointVec[0].invBindposeMatrix[3][1],jointVec[0].invBindposeMatrix[3][2], jointVec[0].invBindposeMatrix[3][3] 
							};

	skinData[0] = bone_global_pose[0] * invBindPose;

	for (int i = 1; i < jointVec.size(); i++)
	{
		// Get all the keyframe vector for the current joint.
		keyVec = keyFrameMap->at(jointVec[i].jointID);
		// Parse input data to correct format.
		glm::vec4 k1Trans = glm::make_vec4(keyVec[k1].translation);
		glm::vec4 k2Trans = glm::make_vec4(keyVec[k2].translation);
		glm::vec4 k1Scale = glm::make_vec4(keyVec[k1].scale);
		glm::vec4 k2Scale = glm::make_vec4(keyVec[k2].scale);
		glm::quat k1Rot = glm::make_quat(keyVec[k1].rotation);
		glm::quat k2Rot = glm::make_quat(keyVec[k2].rotation);
		// Calulate the transformations.
		glm::vec4 translation_r = k1Trans * (1 - t) + k2Trans * t;
		glm::vec4 scaling_r = k1Scale * (1 - t) + k2Scale * t;
		glm::quat quaternion_r = glm::slerp(k1Rot, k2Rot, t);

		// Transfer to matricies
		glm::mat4 idMat = glm::mat4(1.0f);
		glm::mat4 transMat = glm::translate(idMat, glm::vec3(translation_r));
		glm::mat4 scaleMat = glm::scale(idMat, glm::vec3(scaling_r));
		glm::mat4 rotMat = glm::mat4_cast(quaternion_r);
		//Put it in the list.
		bone_global_pose[i] = transMat * rotMat * scaleMat;

		glm::mat4 invBindPose = { jointVec[i].invBindposeMatrix[0][0],jointVec[i].invBindposeMatrix[0][1],jointVec[i].invBindposeMatrix[0][2], jointVec[i].invBindposeMatrix[0][3],
								  jointVec[i].invBindposeMatrix[1][0],jointVec[i].invBindposeMatrix[1][1],jointVec[i].invBindposeMatrix[1][2], jointVec[i].invBindposeMatrix[1][3],
								  jointVec[i].invBindposeMatrix[2][0],jointVec[i].invBindposeMatrix[2][1],jointVec[i].invBindposeMatrix[2][2], jointVec[i].invBindposeMatrix[2][3],
								  jointVec[i].invBindposeMatrix[3][0],jointVec[i].invBindposeMatrix[3][1],jointVec[i].invBindposeMatrix[3][2], jointVec[i].invBindposeMatrix[3][3]
		};
		skinData[i] = bone_global_pose[i] * invBindPose;
	}

}


#include "fypch.hpp"
#include "AnimationHandler.h"

Frosty::AnimationHandler::AnimationHandler()
{
	glm::mat4 iD = glm::mat4 { 1.0 };

	for (int i = 0; i < MAX_BONES; i++)
	{
		skinData.jTrans[i] = iD;
	}
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
	keyFrameMap = *kMap;
}

void Frosty::AnimationHandler::getSkinData(void* &data)
{
	data = &skinData;
}

int Frosty::AnimationHandler::getNrOfJoints()
{
	return jointVec.size();
}

void Frosty::AnimationHandler::CalculateAnimMatrix( float * currentAnimTime)
{
	float duration = animPtr->duration;
	if (*currentAnimTime > animPtr->duration)
	{
		*currentAnimTime = 0;
	}
	//Since all animations are baked which joint it is irrelevant for determining last keyframe.
	std::vector<Luna::Keyframe> keyVec;
	keyVec = keyFrameMap[jointVec[0].jointID];

	// keyframes involved.
	int k1 = (int)(*currentAnimTime * animPtr->fps);
	//keyFrameMap.
	int k2 = fminf(k1 + 1, keyVec[keyVec.size()-1].timePosition);

	// keyframes in anim_time terms
	float k1_time = k1 / animPtr->fps;
	float k2_time = k2 / animPtr->fps;
	// time rescaled into [0..1] as a percentage between k1 and k2
	float t = (*currentAnimTime - k1_time) / (k2_time - k1_time);

	glm::mat4 bone_global_pose[MAX_BONES] = { glm::mat4(1.0) };

	///*glm::vec4 k2Trans = { keyVec[k2].translation[0] ,keyVec[k2].translation[1] ,keyVec[k2].translation[2] ,keyVec[k2].translation[3] };*/

	//TODO: All the make_ calls should be done before this function.
	//Ideally keyVec[k1].translation and the others would already be parsed correctly.
	glm::vec4 k1Trans_r = glm::make_vec4(keyVec[k1].translation);
	glm::vec4 k2Trans_r = glm::make_vec4(keyVec[k2].translation);

	glm::vec4 k1Scale_r = glm::make_vec4(keyVec[k1].scale);
	glm::vec4 k2Scale_r = glm::make_vec4(keyVec[k2].scale);

	prepRotation(keyVec[k1].rotation);
	prepRotation(keyVec[k2].rotation);

	glm::quat k1Rot_r = glm::make_quat(keyVec[k1].rotation);
	glm::quat k2Rot_r = glm::make_quat(keyVec[k2].rotation);

	//Root Values.
	glm::vec3 translation_r = glm::vec3(k1Trans_r) * (1 - t) + glm::vec3(k2Trans_r) * t; //Order of multiplication should not matter here
	glm::vec3 scaling_r		= glm::vec3(k1Scale_r) * (1 - t) + glm::vec3(k2Scale_r) * t; //Same as above
	glm::quat quaternion_r  = glm::slerp(k1Rot_r, k2Rot_r, t); 

	// Transfer to matricies
	glm::mat4 idMat_r = glm::mat4(1.0f);
	glm::mat4 transMat_r = glm::translate(idMat_r, translation_r);
	glm::mat4 scaleMat_r = glm::scale(idMat_r, scaling_r);
	glm::quat wat = glm::vec3(0, 0, 0);
	glm::mat4 rotMat_r =  glm::mat4_cast(wat);
	// Put it in the list.

	/////
	float bindPose[16] = { 0 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bindPose[i * 4 + j] = jointVec[0].invBindposeMatrix[i][j];
		}
	}

	bone_global_pose[0] = transMat_r * rotMat_r * scaleMat_r;

	//glm::mat4 invBindPose_r = glm::make_mat4(&jointVec[0].invBindposeMatrix[0][0]);
	glm::mat4 invBindPose_r = glm::make_mat4(bindPose);

	skinData.jTrans[0] = bone_global_pose[0] * invBindPose_r;
	//skinData.jTrans[1] = bone_global_pose[0] * invBindPose_r;
	//skinData.jTrans[2] = bone_global_pose[0] * invBindPose_r;
	//skinData.jTrans[3] = bone_global_pose[0] * invBindPose_r;

	for (int i = 1; i < jointVec.size(); i++)
	{
		std::vector<Luna::Keyframe> keyVec_i;
		// Get all the keyframe vector for the current joint.
		keyVec_i = keyFrameMap[jointVec[i].jointID];
		// Parse input data to correct format.
		glm::vec4 k1Trans = glm::make_vec4(keyVec_i[k1].translation);
		glm::vec4 k2Trans = glm::make_vec4(keyVec_i[k2].translation);

		glm::vec4 k1Scale = glm::make_vec4(keyVec_i[k1].scale);
		glm::vec4 k2Scale = glm::make_vec4(keyVec_i[k2].scale);

		prepRotation(keyVec_i[k1].rotation);
		prepRotation(keyVec_i[k2].rotation);

		glm::quat k1Rot = glm::make_quat(keyVec_i[k1].rotation);
		glm::quat k2Rot = glm::make_quat(keyVec_i[k2].rotation);
		// Calulate the transformations.
		glm::vec3 translation = glm::vec3(k1Trans) * (1 - t) + glm::vec3(k2Trans) * t;
		glm::vec3 scaling = glm::vec3(k1Scale) * (1 - t) + glm::vec3(k2Scale) * t;
		glm::quat quaternion = glm::slerp(k1Rot, k2Rot, t);

		// Transfer to matricies
		glm::mat4 idMat = glm::mat4(1.0f);
		glm::mat4 transMat = glm::translate(idMat, translation);
		glm::mat4 scaleMat = glm::scale(idMat,scaling);
		glm::mat4 rotMat = glm::mat4_cast(quaternion);
		//Put it in the list.
		bone_global_pose[i] = transMat * rotMat * scaleMat;

		float bindPose2[16] = { 0 };

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				bindPose2[i * 4 + j] = jointVec[i].invBindposeMatrix[i][j];
			}
		}

		//glm::mat4 invBindPose = glm::make_mat4(&jointVec[i].invBindposeMatrix[0][0]);
		glm::mat4 invBindPose = glm::make_mat4(bindPose2);


		bone_global_pose[i] = bone_global_pose[jointVec[i].parentID] * bone_global_pose[i];
		skinData.jTrans[i] = bone_global_pose[i] * invBindPose;
	}
}

//GLM handles quaternions differently than FBX. So the last element must become the first and vice versa.
void Frosty::AnimationHandler::prepRotation(float arr[4])
{
	float temp = arr[3];
	arr[3] = arr[0];
	arr[0] = temp;
	
}


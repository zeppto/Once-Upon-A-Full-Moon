#include"fypch.hpp"


#include<Luna/include/Reader.h>
#include"Glad/glad.h"
#include"Animation.hpp"

namespace Frosty
{

	Animation::~Animation()
	{
	}

	const Luna::Skeleton& Animation::GetSkeleton() const
	{
		return m_Skeleton;
	}

	const Luna::Animation& Animation::GetAnimation() const
	{
		return m_Animation;
	}

	const std::vector<Luna::Joint>& Animation::GetJointVec() const
	{
		return m_Joints;
	}

	const std::vector<Luna::Weights>& Animation::GetWeights() const
	{
		return m_Weights;
	}

	const std::map<uint16_t, std::vector<Luna::Keyframe>>& Animation::GetKeyFrameMap() const
	{
		return m_KeyframeMap;
	}

	const std::vector<Luna::Keyframe>& Animation::GetKeyFrameVec(const uint16_t& jointId) const
	{
		return m_KeyframeMap.at(jointId);
	}

	glm::mat4* Animation::getHoldingJoint()
	{
		if (m_HoldingJoint != nullptr)
		{
			return &m_SkinData.jTrans[*m_HoldingJoint];
		}
		else
		{
			return nullptr;
		}
	}

	glm::vec3* Animation::getHoldingJointOffset()
	{
		return &m_HoldingJointOffset;
	}

	void Animation::GetSkinData(void*& data, int& nrOfJoints)
	{
		data = &m_SkinData;
		nrOfJoints = MAX_BONES;
	}
	
#define Fbx2Hmm(from,to) { for (int ce=0; ce < 16; ce++) to[ce/4][ce%4] = from[ce/4][ce%4]; }

	void Animation::CalculateAnimMatrix(float* currentAnimTime)
	{
		m_StridePercent = *currentAnimTime / m_Animation.duration;

		if (*currentAnimTime >= m_Animation.duration)
		{
			if (isRepeating == true)
			{
				*currentAnimTime = 0;
			}
			else
			{
				isFinished = true;
				m_StridePercent = 0.0f;
				return;
			}
		}
		//Since all animations are baked which joint it is irrelevant for determining last keyframe.
		std::vector<Luna::Keyframe> keyVec;
		keyVec = m_KeyframeMap[m_Joints[0].jointID];

		// keyframes involved.
		int k1 = (int)(*currentAnimTime * m_Animation.fps);
		//keyFrameMap.
		int k2 = (int)glm::min(k1 + 1.0f, keyVec[keyVec.size() - 1].timePosition);
		//int k2 = fminf(k1 + 1.0f, keyVec[keyVec.size() - 1].timePosition);

		// keyframes in anim_time terms
		float k1_time = k1 / m_Animation.fps;
		float k2_time = k2 / m_Animation.fps;
		// time rescaled into [0..1] as a percentage between k1 and k2
		float t = (*currentAnimTime - k1_time) / (k2_time - k1_time);

		glm::mat4 bone_global_pose[MAX_BONES] = { glm::mat4(1.0) };

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
		glm::vec3 scaling_r = glm::vec3(k1Scale_r) * (1 - t) + glm::vec3(k2Scale_r) * t; //Same as above
		glm::quat quaternion_r = glm::slerp(k1Rot_r, k2Rot_r, t);

		// Transfer to matricies
		glm::mat4 idMat_r = glm::mat4(1.0f);
		glm::mat4 transMat_r = glm::translate(idMat_r, translation_r);
		glm::mat4 scaleMat_r = glm::scale(idMat_r, scaling_r);
		glm::mat4 rotMat_r = glm::mat4_cast(quaternion_r);
		// Put it in the list.

		glm::mat4 local_r = transMat_r * rotMat_r * scaleMat_r;

		glm::mat4 invBindPose_r = glm::make_mat4(&m_Joints[0].invBindposeMatrix[0][0]);

		m_SkinData.jTrans[0] = local_r * invBindPose_r;

		bone_global_pose[0] = local_r;

		for (int i = 1; i < m_Joints.size(); i++)
		{
			std::vector<Luna::Keyframe> keyVec_i;
			// Get all the keyframe vector for the current joint.
			keyVec_i = m_KeyframeMap[m_Joints[i].jointID];
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
			glm::mat4 scaleMat = glm::scale(idMat, scaling);
			glm::mat4 rotMat = glm::mat4_cast(quaternion);
			//Put it in the list.
			glm::mat4 local = transMat * rotMat * scaleMat;

			glm::mat4 invBindPose = glm::make_mat4(&m_Joints[i].invBindposeMatrix[0][0]);

			bone_global_pose[i] = bone_global_pose[m_Joints[i].parentID] * local;
			m_SkinData.jTrans[i] =  bone_global_pose[i] * invBindPose;
		}
	}

	void Animation::SetIsRepeating(bool isRepeat)
	{
		this->isRepeating = isRepeat;
	}

	void Animation::SetIsFinished(bool isFinish)
	{
		this->isFinished = isFinish;
	}

	bool Animation::GetIsFinished()
	{
		return isFinished;
	}

	float Animation::GetStridePercent()
	{
		return m_StridePercent;
	}

	void Animation::SetStridePercent(float stride)
	{
		m_StridePercent = stride;
	}

	bool Animation::LoadToMem(const bool& Reload)
	{
		bool returnValue = false;


		if (!m_OnMem || Reload)
		{

			Luna::Reader tempFile;
			if (tempFile.readFile(m_MetaData.FullFilePath.c_str()))
			{

				bool modelHasSkeleton = m_Has_Skeleton;

				if (m_Has_Skeleton)
				{
					tempFile.getWeights(m_Mesh_Id, m_Weights);
					m_Skeleton = tempFile.getSkeleton();
				}

				if (tempFile.animationExist())
				{
					m_Animation = tempFile.getAnimation();
				}

				tempFile.getJoints(m_Joints);

				for (uint16_t i = 0; i < m_Joints.size(); i++)
				{
					std::string tmp(m_Joints[i].jointName);
					//Identifing joint to hold items
					if (tmp == "Wrist_R")
					{
						//Animation translation
						m_HoldingJoint = &m_Joints[i].jointID;
					}
					tempFile.getKeyframes(m_Joints.at(i).jointID, m_KeyframeMap[m_Joints[i].jointID]);
				}

				m_OnMem = true;

			}
			else
			{
				FY_CORE_WARN("Luna Could not Load File: {0}", m_MetaData.FileName);
			}
		}
		else
		{
			FY_CORE_INFO("File: {0}, Is already loaded on Memory", m_MetaData.FileName);
		}
		return !m_OnMem;
	}


bool Animation::DeleteFromMem()
{
	if (m_OnMem)
	{
		m_Joints.erase(m_Joints.begin(), m_Joints.end());
		m_Weights.erase(m_Weights.begin(), m_Weights.end());
		m_KeyframeMap.erase(m_KeyframeMap.begin(), m_KeyframeMap.end());
		m_OnMem = false;
	}
	else
	{
		FY_CORE_INFO("Animation: {0}, Is already deleted from Memory", m_MetaData.FileName);
	}

	return !m_OnMem;
}

bool Animation::LoadToGPU()
{

	//Insert load to gpu here

	return false;
}

bool Animation::DeleteFromGPU()
{
	//Insert load from gpu here

	return false;
}

void Animation::prepRotation(float arr[4])
{
	//float temp = arr[3];
	//arr[3] = arr[0];
	//arr[0] = temp;
	float corr[4] = { 0 };
	corr[0] = arr[3];
	corr[1] = arr[0];
	corr[2] = arr[1];
	corr[3] = arr[2];

	for (int i = 0; i < 4; i++)
	{
		arr[i] = corr[i];
	}
}

}
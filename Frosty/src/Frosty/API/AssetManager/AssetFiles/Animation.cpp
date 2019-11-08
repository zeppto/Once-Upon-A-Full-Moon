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

	void Animation::GetSkinData(void*& data, int& nrOfJoints)
	{
		data = &m_SkinData;
		nrOfJoints = MAX_BONES;
	}

	void Animation::CalculateAnimMatrix(float* currentAnimTime)
	{
		float duration = m_Animation.duration;
		if (*currentAnimTime > m_Animation.duration)
		{
			*currentAnimTime = 0;
		}
		//Since all animations are baked which joint it is irrelevant for determining last keyframe.
		std::vector<Luna::Keyframe> keyVec;
		keyVec = m_KeyframeMap[m_Joints[0].jointID];

		// keyframes involved.
		int k1 = (int)(*currentAnimTime * m_Animation.fps);
		//keyFrameMap.
		int k2 = fminf(k1 + 1, keyVec[keyVec.size() - 1].timePosition);

		// keyframes in anim_time terms
		float k1_time = k1 / m_Animation.fps;
		float k2_time = k2 / m_Animation.fps;
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
		glm::vec3 scaling_r = glm::vec3(k1Scale_r) * (1 - t) + glm::vec3(k2Scale_r) * t; //Same as above
		glm::quat quaternion_r = glm::slerp(k1Rot_r, k2Rot_r, t);

		// Transfer to matricies
		glm::mat4 idMat_r = glm::mat4(1.0f);
		glm::mat4 transMat_r = glm::translate(idMat_r, translation_r);
		glm::mat4 scaleMat_r = glm::scale(idMat_r, scaling_r);
		glm::quat wat = glm::vec3(0, 0, 0);
		glm::mat4 rotMat_r = glm::mat4_cast(wat);
		// Put it in the list.

		/////
		float bindPose[16] = { 0 };

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				bindPose[i * 4 + j] = m_Joints[0].invBindposeMatrix[i][j];
			}
		}

		bone_global_pose[0] = transMat_r * rotMat_r * scaleMat_r;

		//glm::mat4 invBindPose_r = glm::make_mat4(&jointVec[0].invBindposeMatrix[0][0]);
		glm::mat4 invBindPose_r = glm::make_mat4(bindPose);

		m_SkinData.jTrans[0] = bone_global_pose[0] * invBindPose_r;
		//skinData.jTrans[1] = bone_global_pose[0] * invBindPose_r;
		//skinData.jTrans[2] = bone_global_pose[0] * invBindPose_r;
		//skinData.jTrans[3] = bone_global_pose[0] * invBindPose_r;

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
			bone_global_pose[i] = transMat * rotMat * scaleMat;

			float bindPose2[16] = { 0 };

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					bindPose2[i * 4 + j] = m_Joints[i].invBindposeMatrix[i][j];
				}
			}

			//glm::mat4 invBindPose = glm::make_mat4(&jointVec[i].invBindposeMatrix[0][0]);
			glm::mat4 invBindPose = glm::make_mat4(bindPose2);

			bone_global_pose[i] = bone_global_pose[m_Joints[i].parentID] * bone_global_pose[i];
			m_SkinData.jTrans[i] = bone_global_pose[i] * invBindPose;
		}
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
					/*tempFile.getKeyframes();*/

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
	float temp = arr[3];
	arr[3] = arr[0];
	arr[0] = temp;
}

}
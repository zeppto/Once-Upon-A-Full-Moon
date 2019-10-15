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
					tempFile.getKeyframes(m_Joints.at(i).jointID, m_KeyframeMap.at(m_Joints.at(i).jointID));
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

}
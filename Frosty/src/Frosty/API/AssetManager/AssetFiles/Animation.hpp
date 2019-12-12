#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include"AssetFile.hpp"
#include<Luna/include/Luna.h>
#include "glm/gtx/quaternion.hpp"

//CAREFUL: If changed. Change in animationVS too!
#define MAX_BONES 128

namespace Frosty
{

	class Animation : public AssetFile
	{

	public:
		struct JointTransforms
		{
			glm::mat4 jTrans[MAX_BONES];
		};

	private:

		uint16_t m_Mesh_Id;
		bool m_Has_Skeleton;

		Luna::Skeleton m_Skeleton;
		Luna::Animation m_Animation;
		std::vector<Luna::Joint> m_Joints;
		std::vector<Luna::Weights> m_Weights;
		std::map<uint16_t, std::vector<Luna::Keyframe>> m_KeyframeMap;
		unsigned int* m_HoldingJoint;
		glm::vec3 m_HoldingJointOffset;
		bool isRepeating;
		bool isFinished;
		float m_StridePercent;

		JointTransforms m_SkinData;


	public:
		inline Animation() : m_Mesh_Id(-1),
			isRepeating(true),
			isFinished(false),
			m_Has_Skeleton(false),
			m_HoldingJoint(nullptr),
			m_HoldingJointOffset(0.0f),
			m_Skeleton(),
			m_Joints(),
			m_Weights(),
			m_KeyframeMap(),
			m_SkinData(),
			AssetFile(FileMetaData()) {}

		inline Animation(const FileMetaData& MetaData, const uint16_t& MeshId, Luna::Animation LuAni, const bool& HasSkeleton = false)
			:m_Mesh_Id(MeshId),
			m_Has_Skeleton(HasSkeleton),
			m_Animation(LuAni),
			isRepeating(true),
			isFinished(false),
			m_HoldingJoint(nullptr),
			m_HoldingJointOffset(0.0f),
			m_Skeleton(),
			m_Joints(),
			m_Weights(),
			m_SkinData(),
			m_KeyframeMap(),
			AssetFile(MetaData)
		{
			for (int i = 0; i < MAX_BONES ; i++)
			{
				m_SkinData.jTrans[i] = glm::mat4(1.0f);
			}

		}
		virtual ~Animation();


		const Luna::Skeleton& GetSkeleton()const;
		const Luna::Animation& GetAnimation()const;
		const std::vector<Luna::Joint>& GetJointVec() const;
		const std::vector<Luna::Weights>& GetWeights()const;
		const std::map<uint16_t, std::vector<Luna::Keyframe>>& GetKeyFrameMap()const;
		const std::vector<Luna::Keyframe>& GetKeyFrameVec(const uint16_t& jointId)const;
		glm::mat4* getHoldingJoint();
		glm::vec3* getHoldingJointOffset();

		void GetSkinData(void*& data, int &nrOfJoints);
		void CalculateAnimMatrix(float* currentAnimTime);
		void SetIsRepeating(bool isRepeat);
		void SetIsFinished(bool isFinish);
		bool GetIsFinished();
		float GetStridePercent();
		void SetStridePercent(float stride);
		inline const std::string GetName()const { return m_Animation.animationName;}

		virtual bool LoadToMem(const bool& Reload = false);
		virtual bool DeleteFromMem();

		virtual bool LoadToGPU();
		virtual bool DeleteFromGPU();


	private:
		void prepRotation(float arr[4]);


	};

}
#endif // !ANIMATION_HPP


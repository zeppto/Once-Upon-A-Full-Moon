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
		unsigned int* m_holdingJoint;

		JointTransforms m_SkinData;


	public:
		Animation() : AssetFile(FileMetaData()) {}
		Animation(const FileMetaData& MetaData, const uint16_t& MeshId, Luna::Animation LuAni, const bool& HasSkeleton = false)
			:m_Mesh_Id(MeshId), m_Has_Skeleton(HasSkeleton), AssetFile(MetaData), m_Animation(LuAni)
		{
			for (int i = 0; i < MAX_BONES ; i++)
			{
				m_SkinData.jTrans[i] = glm::mat4(1.0f);
			}
			m_holdingJoint = nullptr;
		}
		virtual ~Animation();


		const Luna::Skeleton& GetSkeleton()const;
		const Luna::Animation& GetAnimation()const;
		const std::vector<Luna::Joint>& GetJointVec() const;
		const std::vector<Luna::Weights>& GetWeights()const;
		const std::map<uint16_t, std::vector<Luna::Keyframe>>& GetKeyFrameMap()const;
		const std::vector<Luna::Keyframe>& GetKeyFrameVec(const uint16_t& jointId)const;
		const glm::mat4* getHoldingJoint();

		void GetSkinData(void*& data, int &nrOfJoints);
		void CalculateAnimMatrix(float* currentAnimTime);

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


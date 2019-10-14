#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include"AssetFile.hpp"
#include<Luna/include/Luna.h>

namespace Frosty
{

	class Animation : public AssetFile
	{

	public:

	private:

		uint16_t m_Mesh_Id;
		bool m_Has_Skeleton;

		Luna::Skeleton m_Skeleton;
		Luna::Animation m_Animation;;
		std::vector<Luna::Joint> m_Joints;
		std::vector<Luna::Weights> m_Weights;
		std::map<uint16_t, std::vector<Luna::Keyframe>> m_KeyframeMap;


	public:
		inline Animation() : AssetFile(FileMetaData()) {}
		inline Animation(const FileMetaData& MetaData, const uint16_t& MeshId, const bool& HasSkeleton = false)
			:m_Mesh_Id(MeshId), m_Has_Skeleton(HasSkeleton), AssetFile(MetaData) {}
		virtual ~Animation();


		const Luna::Skeleton& GetSkeleton()const;
		const Luna::Animation& GetAnimation()const;
		const std::vector<Luna::Joint>& GetJointVec() const;
		const std::vector<Luna::Weights>& GetWeights()const;
		const std::map<uint16_t, std::vector<Luna::Keyframe>>& GetKeyFrameMap()const;
		const std::vector<Luna::Keyframe>& GetKeyFrameVec(const uint16_t& jointId)const;

		inline const std::string GetName()const { return m_Animation.animationName;}

		virtual bool LoadToMem(const bool& Reload = false);
		virtual bool DeleteFromMem();

		virtual bool LoadToGPU();
		virtual bool DeleteFromGPU();


	private:



	};

}
#endif // !ANIMATION_HPP


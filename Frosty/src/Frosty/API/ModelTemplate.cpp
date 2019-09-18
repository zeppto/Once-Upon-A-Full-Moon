#include "fypch.hpp"
#include "ModelTemplate.hpp"


namespace Frosty
{
	ModelTemplate::ModelTemplate()
	{
	}
	ModelTemplate::~ModelTemplate()
	{
	}
	ModelTemplate::ModelTemplate(const ModelTemplate& other)
	{
	}
	ModelTemplate& ModelTemplate::operator=(ModelTemplate& other)
	{
		// TODO: insert return statement here
	}
	Luna::Skeleton* Frosty::ModelTemplate::getSkeleton()
	{
		return &m_Skeleton;
	}

	Luna::Animation* ModelTemplate::getAnimation()
	{
		return &m_Animation;
	}

	std::vector<Luna::Mesh>* ModelTemplate::getMeshVector()
	{
		return &m_Meshes;
	}

	std::unordered_map<uint16_t, ModelTemplate::MeshInfo>* ModelTemplate::getMeshInfoMap()
	{
		return &m_MeshInfoMap;
	}

	std::unordered_map<uint16_t, std::vector<Luna::Keyframe>>* ModelTemplate::getKeyframeMap()
	{
		return &m_KeyframeMap;
	}

	ModelTemplate::MeshInfo* ModelTemplate::getMeshInfo(uint16_t meshId)
	{
		return &m_MeshInfoMap[meshId];
	}

	std::vector<Luna::Keyframe>* ModelTemplate::getKeyframes(uint16_t jointId)
	{
		return &m_KeyframeMap[jointId];
	}


	std::vector<Luna::Joint>* ModelTemplate::getJointVector()
	{
		return &m_Joints;
	}

	//std::vector<Luna::BoundingBox>* ModelTemplate::getBoundingBoxVector()
	//{
	//	return &m_BoundingBoxes;
	//}

	//std::vector<Luna::Vertex*>* ModelTemplate::getVertexVector()
	//{
	//	return &m_MeshVertices;
	//}

	//std::vector<Luna::Index*>* ModelTemplate::getIndicesVector()
	//{
	//	return &m_MeshIndices;
	//}

	//std::vector<Luna::Weights*>* ModelTemplate::getWeightsVector()
	//{
	//	return &m_Weights;
	//}

	//std::vector<Luna::Keyframe*>* ModelTemplate::getKeyframeVector()
	//{
	//	return &m_Keyframes;
	//}

}

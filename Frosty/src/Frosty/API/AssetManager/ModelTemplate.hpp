#ifndef MODELTEMPLATE_H
#define MODELTEMPLATE_H
#include<Luna/include/Luna.h>


namespace Frosty
{


	


	class ModelTemplate
	{

	//Priv Struct
	public:
		struct MeshInfo
		{

			MeshInfo() {};
			MeshInfo(
				Luna::BoundingBox BoundingBox,
				std::vector<Luna::Vertex> MeshVertices,
				std::vector<Luna::Index> MeshIndices,
				std::vector<Luna::Weights> Weights
			)
				: BoundingBox(BoundingBox),
				MeshVertices(MeshVertices),
				MeshIndices(MeshIndices),
				Weights(Weights) {};

			//	std::vector<Luna::Material> materials;
			Luna::BoundingBox BoundingBox;
			std::vector<Luna::Vertex> MeshVertices;
			std::vector<Luna::Index> MeshIndices;
			std::vector<Luna::Weights> Weights;
		};




	//Variables
	public:



	//Variables
	private:
		uint16_t m_SelfID;
		static uint16_t s_ModelTemplateID;
		static uint16_t s_ModelTemplateCounter;


		Luna::Skeleton m_Skeleton;
		Luna::Animation m_Animation;

		std::vector<Luna::Mesh> m_Meshes;
		std::vector<Luna::Joint> m_Joints;
		
		std::map<uint16_t, MeshInfo> m_MeshInfoMap;
		std::map<uint16_t, std::vector<Luna::Keyframe>> m_KeyframeMap;


	//Funcs
	public:
		ModelTemplate();
		virtual ~ModelTemplate();
		ModelTemplate(const ModelTemplate& other);
		ModelTemplate& operator =(const ModelTemplate& other);
		bool operator == (const ModelTemplate& other);
		bool operator == (const uint16_t& otherID);

		//maybe needed?
		//void set(uint16_t meshID, )
		Luna::Skeleton* GetSkeleton();
		Luna::Animation* GetAnimation();

		std::vector<Luna::Mesh>* GetMeshVector();
		std::vector<Luna::Joint>* GetJointVector();

		ModelTemplate::MeshInfo * GetMeshInfo(const uint16_t& meshId);
		std::vector<Luna::Keyframe>* GetKeyframes(const uint16_t& jointId);

		std::map<uint16_t, MeshInfo>* GetMeshInfoMap();
		std::map<uint16_t, std::vector<Luna::Keyframe>>* GetKeyframeMap();





		const Luna::Skeleton* GetSkeletonConst() { return &m_Skeleton; }
		const Luna::Animation* GetAnimationConst() { return &m_Animation; };

		const std::vector<Luna::Mesh>* GetMeshVectorConst() { return &m_Meshes; }
		const std::vector<Luna::Joint>* GetJointVectorConst() { return &m_Joints; }

		const ModelTemplate::MeshInfo* GetMeshInfoConst(const uint16_t& meshId) { return &m_MeshInfoMap[meshId];}
		const std::vector<Luna::Keyframe>* GetKeyframesConst(const uint16_t& jointId) { return &m_KeyframeMap[jointId];}

		const std::map<uint16_t, MeshInfo>* GetMeshInfoMapConst() { return &m_MeshInfoMap; }
		const std::map<uint16_t, std::vector<Luna::Keyframe>>* GetKeyframeMapConst() { return &m_KeyframeMap; }

		
		const uint16_t& GetId() const;
		static const uint16_t& GetNumberOfModelTemplates();



		void AddMeshInfo(
			const uint16_t& meshId,
			const Luna::BoundingBox& BoundingBox,
			const std::vector<Luna::Vertex>& MeshVertices,
			const std::vector<Luna::Index>& MeshIndices,
			const std::vector<Luna::Weights>& Weights);





		

		//std::vector<Luna::BoundingBox>* getBoundingBoxVector();
		//std::vector<Luna::Vertex*>* getVertexVector();
		//std::vector<Luna::Index*>* getIndicesVector();
		//std::vector<Luna::Weights*>* getWeightsVector();
		//std::vector<Luna::Keyframe*>* getKeyframeVector();



	//Funcs
	private:

		


	};

}

#endif // !MODELTEMPLATE_H


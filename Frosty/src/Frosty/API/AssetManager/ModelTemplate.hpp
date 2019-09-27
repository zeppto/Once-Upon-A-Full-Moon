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
				: m_BoundingBox(BoundingBox),
				m_MeshVertices(MeshVertices),
				m_MeshIndices(MeshIndices),
				m_Weights(Weights) {};

			//	std::vector<Luna::Material> materials;
			Luna::BoundingBox m_BoundingBox;
			std::vector<Luna::Vertex> m_MeshVertices;
			std::vector<Luna::Index> m_MeshIndices;
			std::vector<Luna::Weights> m_Weights;
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
		
		std::unordered_map<uint16_t, MeshInfo> m_MeshInfoMap;
		std::unordered_map<uint16_t, std::vector<Luna::Keyframe>> m_KeyframeMap;


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

		std::unordered_map<uint16_t, MeshInfo>* GetMeshInfoMap();
		std::unordered_map<uint16_t, std::vector<Luna::Keyframe>>* GetKeyframeMap();
		
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


#ifndef  MESH_HPP
#define MESH_HPP
#include"AssetFile.hpp"
#include"Luna/include/Luna.h"


namespace Frosty
{

	class Mesh : public AssetFile
	{

	public:

	private:

		Luna::Mesh m_LunaMesh;
		Luna::BoundingBox m_BoundingBox;
		std::vector<Luna::Index> m_MeshIndices;
		std::vector<Luna::Vertex> m_MeshVertices;


	public:

		inline Mesh() : m_LunaMesh(Luna::Mesh()), AssetFile(FileMetaData()) {}
		inline Mesh(const FileMetaData& MetaData, const Luna::Mesh& LunMesh) : m_LunaMesh(LunMesh), AssetFile(MetaData) {}
		inline virtual ~Mesh() { DeleteFromGPU(); }
		
		inline const Luna::Mesh GetLunaMesh() {return m_LunaMesh;}
		inline const std::vector<Luna::Vertex>& getVertexVec() { return m_MeshVertices; }
		inline const std::vector<Luna::Index>& getIndexVec() { return m_MeshIndices; }


		virtual bool LoadToMem(const bool& Reload = false);
		virtual bool DeleteFromMem();

		virtual bool LoadToGPU();
		virtual bool DeleteFromGPU();


	private:



	};
}


#endif // ! MESH_HPP

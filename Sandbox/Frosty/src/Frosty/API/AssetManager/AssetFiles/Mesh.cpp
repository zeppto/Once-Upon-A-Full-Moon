#include"fypch.hpp"
#include"Mesh.hpp"
#include "Luna/include/Reader.h"
#include"Glad/glad.h"



#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{

	bool Mesh::LoadToMem(const bool& Reload)
	{

		bool returnValue = false;

		Luna::Reader tempFile;

		if (!m_OnMem || Reload)
		{
			if (tempFile.readFile(m_MetaData.FullFilePath.c_str()))
			{
				if (m_LunaMesh.hasBoundingBox)
				{
					m_BoundingBox = tempFile.getBoundingBox(m_LunaMesh.id);
				}
				tempFile.getIndices(m_LunaMesh.id, m_MeshIndices);
				tempFile.getVertices(m_LunaMesh.id, m_MeshVertices);
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
		return returnValue;
	}

	bool Mesh::DeleteFromMem()
	{
		if (m_OnMem)
		{
			//Leav the bounding box alooone
			m_MeshIndices.erase(m_MeshIndices.begin(), m_MeshIndices.end());
			m_MeshVertices.erase(m_MeshVertices.begin(), m_MeshVertices.end());
			m_OnMem = false;
		}

		return !m_OnMem;
	}

	bool Mesh::LoadToGPU()
	{
		if (m_OnGPU)
		{
			DeleteFromGPU();
		}

		glGenVertexArrays(1, &m_GPU_Data_ID);
		glBindVertexArray(m_GPU_Data_ID);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &m_GPU_Data_ID);
		glBindBuffer(GL_ARRAY_BUFFER, m_GPU_Data_ID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Luna::Vertex) * m_LunaMesh.vertexCount, m_MeshVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 5));

		glBindVertexArray(0);

		return true;
	}

	bool Mesh::DeleteFromGPU()
	{
		m_OnGPU = false;
		//More buffers?
		glDeleteBuffers(1, &m_GPU_Data_ID);
		return true;
	}

}

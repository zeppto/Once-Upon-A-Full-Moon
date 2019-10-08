#include "fypch.hpp"
#include "ModelTemplate.hpp"
#include"Glad/glad.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
	uint16_t ModelTemplate::s_ModelTemplateID = 0;
	uint16_t ModelTemplate::s_ModelTemplateCounter = 0;

	ModelTemplate::ModelTemplate()
	{
		m_Dumped_Info = false;
		m_SelfID = s_ModelTemplateID++;
		s_ModelTemplateCounter++;
	}
	ModelTemplate::~ModelTemplate()
	{
		m_MeshInfoMap.erase(m_MeshInfoMap.begin(), m_MeshInfoMap.end());
		m_Meshes.erase(m_Meshes.begin(), m_Meshes.end());
		m_Joints.erase(m_Joints.begin(), m_Joints.end());
		m_KeyframeMap.erase(m_KeyframeMap.begin(), m_KeyframeMap.end());
		s_ModelTemplateCounter--;
	}
	ModelTemplate::ModelTemplate(const ModelTemplate& other)
	{
		if (this != &other)
		{
			m_SelfID = s_ModelTemplateID++;
			s_ModelTemplateCounter++;
		}
		m_Dumped_Info = other.m_Dumped_Info;
		m_Skeleton = other.m_Skeleton;
		m_Animation = other.m_Animation;
		m_Meshes = other.m_Meshes;
		m_Joints = other.m_Joints;
		m_MeshInfoMap = other.m_MeshInfoMap;
		m_KeyframeMap = other.m_KeyframeMap;

	}
	ModelTemplate& ModelTemplate::operator=(const ModelTemplate& other)
	{
		// TODO: insert return statement here
		if (this != &other)
		{
			m_SelfID = s_ModelTemplateID++;
			s_ModelTemplateCounter++;
		}

		m_Skeleton = other.m_Skeleton;
		m_Animation = other.m_Animation;
		m_Meshes = other.m_Meshes;
		m_Joints = other.m_Joints;
		m_MeshInfoMap = other.m_MeshInfoMap;
		m_KeyframeMap = other.m_KeyframeMap;


		return *this;
	}
	bool ModelTemplate::operator==(const ModelTemplate& other)
	{		
		// What to compare?
		return false;
	}
	bool ModelTemplate::operator==(const uint16_t& otherID)
	{
		return (otherID == m_SelfID);
	}
	Luna::Skeleton* Frosty::ModelTemplate::GetSkeleton()
	{
		return &m_Skeleton;
	}
	Luna::Animation* ModelTemplate::GetAnimation()
	{
		return &m_Animation;
	}
	std::vector<Luna::Mesh>* ModelTemplate::GetMeshVector()
	{
		return &m_Meshes;
	}
	std::map<uint16_t, ModelTemplate::MeshInfo>* ModelTemplate::GetMeshInfoMap()
	{
		return &m_MeshInfoMap;
	}
	std::map<uint16_t, std::vector<Luna::Keyframe>>* ModelTemplate::GetKeyframeMap()
	{
		return &m_KeyframeMap;
	}
	const uint16_t& ModelTemplate::GetId() const
	{
		return m_SelfID;
	}
	const uint16_t& ModelTemplate::GetNumberOfModelTemplates() 
	{
		return s_ModelTemplateCounter;
	}
	bool  ModelTemplate::LoadModelToGpu(const bool& DumpData)
	{
		bool returnValue = true;
		if (!m_Dumped_Info)
		{
			for (uint8_t i = 0; i < m_Meshes.size(); i++)
			{
				std::shared_ptr<VertexBuffer> tempVertexBuffer;
				tempVertexBuffer.reset(VertexBuffer::Create(&m_MeshInfoMap[i].MeshVertices, m_Meshes.at(i).vertexCount));
								
				BufferLayout layout =
				{
					{ ShaderDataType::Float3, "vsInPos" },
					{ ShaderDataType::Float2, "vsInUV" },
					{ ShaderDataType::Float3, "vsInNorm"}
				};

				tempVertexBuffer->SetLayout(layout);

				std::shared_ptr<VertexArray> TempVertexArray;
				TempVertexArray.reset(VertexArray::Create());
				
				TempVertexArray->Bind();
				TempVertexArray->AddVertexBuffer(tempVertexBuffer);
				TempVertexArray->Unbind();

				m_VertexArrays.emplace_back(TempVertexArray);


				std::vector<uint32_t> indices;
				unsigned int indexCounter = 0;
				int index = -1;

				float pos[3];
				float uv[2];
				float norm[3];






				/*

				
			

				for (size_t i = 0; i < tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.size(); i++)
				{
					index = -1;
					pos[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[0];
					pos[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[1];
					pos[2] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).position[2];

					uv[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[1];
					uv[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).uv[0];

					norm[0] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[0];
					norm[1] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[1];
					norm[2] = tempAssetsManager->GetModeltemplateMetaData(filename)->GetData()->GetMeshInfoMap()->at(0).MeshVertices.at(i).normal[2];

					for (size_t j = 0; j < vertices.size() && index != -1; j++)
					{
						if (vertices.at(j).Position == glm::vec3(pos[0], pos[1], pos[2]) &&
							vertices.at(j).Texture == glm::vec2(uv[0], uv[1]) &&
							vertices.at(j).Normal == glm::vec3(norm[0], norm[1], norm[2]))
						{
							index = j;
						}
					}

					if (index == -1)
					{
						indices.emplace_back(indexCounter++);
						vertices.emplace_back(TempVertex({ pos[0], pos[1], pos[2] }, { uv[0], uv[1] }, { norm[0], norm[1], norm[2] }));
					}
					else
					{
						indices.emplace_back(index);
					}
				}

				std::shared_ptr<VertexBuffer> m_VertexBuffer;
				m_VertexBuffer.reset(VertexBuffer::Create(&vertices.front(), sizeof(TempVertex) * (uint64_t)vertices.size()));

				BufferLayout layout =
				{
					{ ShaderDataType::Float3, "a_Position" },
					{ ShaderDataType::Float2, "a_TexCoord" },
					{ ShaderDataType::Float3, "a_Normal" }
				};

				m_VertexBuffer->SetLayout(layout);
				m_VertexArray->AddVertexBuffer(m_VertexBuffer);		

				std::shared_ptr<IndexBuffer> m_IndexBuffer;
				m_IndexBuffer.reset(IndexBuffer::Create(&indices.front(), indices.size()));
				m_VertexArray->SetIndexBuffer(m_IndexBuffer);*/
				/*unsigned int TempID = -1;				

				glGenVertexArrays(1, &TempID);
				glBindVertexArray(TempID);

				glEnableVertexAttribArray(0);
				glEnableVertexAttribArray(1);
				glEnableVertexAttribArray(2);

				glGenBuffers(1, &TempID);
				glBindBuffer(GL_ARRAY_BUFFER, TempID);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Luna::Vertex) * m_Meshes.at(i).vertexCount, m_MeshInfoMap[i].MeshVertices.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(0));
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 3));
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Luna::Vertex), BUFFER_OFFSET(sizeof(float) * 5));

				glBindVertexArray(0);

				temp_VBO_Vector.emplace_back(TempID);*/
			}

			//Erase all loaded data here
			if (DumpData)
			{
				m_Dumped_Info = true;
			}

			//m_VBOs = temp_VBO_Vector;
		}
		else
		{
			returnValue = false;
			FY_CORE_WARN("Could not load ModelTemplate into Gpu, The Data has been Released");
		}
		return returnValue;
	}
	ModelTemplate::MeshInfo* ModelTemplate::GetMeshInfo(const uint16_t& meshId)
	{
		return &m_MeshInfoMap[meshId];
	}
	std::vector<Luna::Keyframe>* ModelTemplate::GetKeyframes(const uint16_t& jointId)
	{
		return &m_KeyframeMap[jointId];
	}
	std::vector<Luna::Joint>* ModelTemplate::GetJointVector()
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

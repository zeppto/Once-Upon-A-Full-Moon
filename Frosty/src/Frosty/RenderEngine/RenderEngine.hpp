#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"
#include "Frosty/API/Transform.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
	struct TempRender
	{

		std::shared_ptr<ModelTemplate> model_ptr;
		std::shared_ptr<LinkedMaterial> material_Ptr;
		glm::mat4 worldPosition;
		glm::vec3 m_Render_Colour = glm::vec3(1.0f, 1.0f, 1.0f);

		bool m_Render = true;

	};
	enum ShaderProgramsEnum
	{
		TEST_SHAPE = 0,
		FIRSTPASS = 1,
		SECONDPASS = 2
	};

	class RenderEngine
	{
	private:
		//Window Information 
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;
		Camera m_Camera;

		Transform m_Transform; //Testing 
		float m_Rotation = 0; //Testing 
		std::vector<TempRender*> m_Temp_RenderList;

		

		std::vector<unsigned int> m_ShaderProgramVector;
		unsigned int m_testTriangleVBO = 0;
	private:
		//Update Functions
		void UpdateInfoFromWindow();

		//Render Functions	

		//Create Functions 
		void InitBuffers();
		
		void CreateAllShaderPrograms();
		void CreateShaderProgram(std::string VertexShaderPath, std::string FragmentShaderPath, ShaderProgramsEnum Program);

		

	public:

	public:
		RenderEngine();
		virtual ~RenderEngine();

		void ClearColor();
		void Render();
		void UpdateCamera();

		// For Testing
		void CreateTriangle();
		void RenderTriangle();
		void RenderModel(const unsigned int& VBO, const unsigned int& nrOfVertices, const glm::mat4& modelMatrix, const unsigned int& textureID);
		void RenderAllPrefabs();
		void RenderPrefab(std::string prefabName);
		void AddToRenderList(TempRender* obj);
		void RenderTempModels();
	};
}
#endif 
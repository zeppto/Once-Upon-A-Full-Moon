#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"
#include "Frosty/API/Transform.h"
#include "Frosty/UI/Sprite.h"
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
		TEST_LIGHT = 1,
		TEST_TEXTURE = 2,
		FIRSTPASS = 3,
		SECONDPASS = 4,
		THIRDPASS = 5
	};

	class RenderEngine
	{
	private:
		//Window Information 
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;		
		Camera* m_Camera = nullptr;

		Sprite m_Sprite;
		Transform m_Transform;		//Testing 
		float m_Rotation = 0; //Testing 
		//glm::vec3 m_MouseRay{ 0, 0, 0 };

		std::vector<TempRender*> m_Temp_RenderList;

		//FrameBuffers 		
		unsigned int m_FirstPassFramebuffer = 0;
		unsigned int m_GUIPassFramebuffer = 0;

		//Render Textures 
		unsigned int m_ColourRenderTexture = 0;
		unsigned int m_DepthRenderTexture = 0;

		unsigned int m_GUIRenderTexture = 0;

		std::vector<unsigned int> m_ShaderProgramVector;
		unsigned int m_QuadVbo = 0;

		// For Testing 
		bool m_TestMode = false;		
		unsigned int m_testTriangleVBO = 0;

	private:
		//Update Functions
		void UpdateInfoFromWindow();

		//Render Functions
		void RenderPassOne();
		void RenderPassQuad();
		void RenderPassGUI();
		//Create Functions 
		void InitBuffers();
		void CreateFirstPassFrameBuffer();
		void CreateGUIPassFramebuffer();
		void CreateAllShaderPrograms();
		void CreateShaderProgram(std::string VertexShaderPath, std::string FragmentShaderPath, ShaderProgramsEnum Program);

		void CreateQuad();
		//For Testing
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
		void RenderTestTriangle();
		void RenderModel(const unsigned int& VBO, const unsigned int& nrOfVertices, const glm::mat4& modelMatrix, const unsigned int& textureID);
		void RenderAllPrefabs();
		void RenderPrefab(std::string prefabName);
		void AddToRenderList(TempRender* obj);
		void RenderTempModels();
	};
}
#endif 

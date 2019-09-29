#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/API/AssetManager/Assetmanager.hpp"
#include "Frosty/API/Transform.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{	
	enum ShaderProgramsEnum
	{
		TEST_SHAPE = 0,
		TEST_LIGHT = 1,
		FIRSTPASS = 2,	//Render Game
		SECONDPASS = 3, //Render Gui
		THIRDPASS = 4	//Render Quad
	};
	
	class RenderEngine
	{
	private:
		//Window Information 
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;		
		Camera* m_Camera = nullptr;

		Transform m_Transform;		//Testing 
		float m_Rotation = 0; //Testing 
		//glm::vec3 m_MouseRay{ 0, 0, 0 };

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
		bool m_RenderTestModel = true;
		int m_VertexSizeOfTestModel = 0;
		unsigned int m_testModelVBO = 0;
		unsigned int m_testModelTexture = 0;


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

		void Render();
		void UpdateCamera();
		
		// For Testing
		void CreateTriangle();		
		void RenderTestTriangle();
		void CreateTestModelData(std::shared_ptr<ModelTemplate> testModel);
		void CreateTestTextureData(unsigned char* Texture_Data, TextureFile& Texture_File);


	};
}
#endif 

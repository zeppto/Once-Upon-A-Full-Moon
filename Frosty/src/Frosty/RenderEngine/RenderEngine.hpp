#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{	
	enum ShaderProgramsEnum
	{
		TEST_SHAPE = 0,
		TEST_LIGHT = 1,
		FIRSTPASS = 2,
		SECONDPASS = 3
	};
	
	class RenderEngine
	{
	private:
		//Window Information 
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;		
		Camera* m_Camera = nullptr;
		//glm::vec3 m_MouseRay{ 0, 0, 0 };

		//FrameBuffers 		
		unsigned int m_FirstPassFramebuffer = 0;

		//Render Textures 
		unsigned int m_PositionRenderTexture = 0;
		unsigned int m_NormalRenderTexture = 0;
		unsigned int m_AlbedoRenderTexture = 0;
		unsigned int m_AlbedoOrginalTexture = 0;
		unsigned int m_DepthRenderTexture = 0;
		unsigned int m_FinalTexture = 0;

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
		void RenderPassTwo();
		//Create Functions 
		void InitBuffers();
		void CreateFirstPassFrameBuffer();
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
	};
}
#endif 

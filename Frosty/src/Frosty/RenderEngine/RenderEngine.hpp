#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/RenderEngine/VertexArray.hpp"
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

		//FrameBuffers 		
		unsigned int m_FirstPassFramebuffer = 0;

		//Render Textures 		
		std::vector<unsigned int> m_ShaderProgramVector;		
		unsigned int m_QuadVbo = 0;

		// For Testing
		bool m_TestMode = false;	
		unsigned int m_testTriangleVBO = 0;

	private:
		//Update Functions
		void UpdateInfoFromWindow();

		//Render Functions

		//Create Functions 		
		void CreateAllShaderPrograms();
		void CreateShaderProgram(std::string VertexShaderPath, std::string FragmentShaderPath, ShaderProgramsEnum Program);

		//For Testing
	public:

	public:
		RenderEngine();
		virtual ~RenderEngine();

		void SetClearColor(const glm::vec4& color);
		void Clear();
		void DrawIndexed(const std::shared_ptr<VertexArray>& VertexArray);

		void Render();
		void UpdateCamera();
		
		// For Testing
		void CreateTriangle();		
		void RenderTestTriangle();
	};
}
#endif

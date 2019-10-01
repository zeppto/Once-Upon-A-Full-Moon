#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
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
	};
}
#endif 
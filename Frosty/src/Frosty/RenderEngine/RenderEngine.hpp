#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/Camera.hpp"
#include "Frosty/Core/TestBoxModel.hpp"
#include "Frosty/Core/CollisionDetection.hpp"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{	
	enum ShaderProgramsEnum
	{
		TEST_SHAPE = 0
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

		//Render Textures 

		std::vector<unsigned int> m_ShaderProgramVector;		

		// For Testing 
		bool m_TestMode = false;		
		unsigned int m_testTriangleVBO = 0;
		TestBoxModel m_TestBox;
		TestBoxModel m_TestBox2;
		TestBoxModel m_TestPlayer;
		CollisionDetection m_CollisionDetection;

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

		void Render();	
		
		// For Testing
		void CreateTestModel( glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
		void RenderAllTestModels();
		void RenderTestModel();
		void UpdateCamera();
	};
}
#endif 

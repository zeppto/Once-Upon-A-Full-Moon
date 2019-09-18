#ifndef RENDERENGINE_HPP
#define RENDERENGINE_HPP
#include "Frosty/Core/Camera/TestCamera.h"
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{	
	enum ShaderProgramsEnum
	{
		TEST_SHAPE,
		TEST_LIGHT
	};
		
	class RenderEngine
	{
	private:
		//Window Information 
		unsigned int m_WindowWidth = 0;
		unsigned int m_WindowHeight = 0;
		TestCamera* m_TheCamera = nullptr;
		//glm::vec3 m_MouseRay{ 0, 0, 0 };

		//FrameBuffers 
		unsigned int m_FirstPassFramebuffer = 0;
		unsigned int m_ShadowMapFBO = 0;

		//Render Textures 

		std::vector<unsigned int> m_ShaderProgramVector;		

		// For Testing 
		bool m_TestMode = false;		
		unsigned int m_testTriangleVBO = 0;

	private:
		//Update Functions
		void UpdateInfoFromWindow();

		//Render Functions
		//void RenderModelTemplate(ModelTemplate& modelTemplate, ObjectHolder& GameObjectVector);
		//void RenderObject(const glm::mat4& UniformMatrix, const unsigned int& ModelSize);
		//void RenderPassShadow();
		//void RenderModelTemplateShadows(ModelTemplate& modelTemplate, ObjectHolder& GameObjectVector);
		//void RenderShadows();
		//void RenderPassTwo();
		//void RenderRectangles();
		//void RenderPointLights();
		//void RenderPassOne();
		//void RenderLightSpheres();

		//Create Functions 
		void CreateAllShaderPrograms();
		void CreateShaderProgram(std::string VertexShaderPath, std::string FragmentShaderPath, ShaderProgramsEnum Program);

		//For Testing
	public:

	public:
		RenderEngine();
		virtual ~RenderEngine();

		void Render();
		//void InitiateStaticModelTemplates();
		void ClearBackground();

		//void InitiateShape();
		//void AddTextureToShape(TextureInfo& textureInfo);
		//void DestroyTexture(TextureInfo& textureInfo);

		//inline void SetRenderPointLights(const bool& aBool) { m_RenderPointLights = aBool; }

		// For Testing
		void CreateTriangle();		
		void RenderTestTriangle();
	};
}
#endif 

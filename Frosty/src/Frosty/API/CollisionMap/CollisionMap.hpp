#ifndef COLLISIONMAP_HPP
#define COLLISIONMAP_HPP
#include"..\..\Core\Camera\Camera.hpp"

namespace Frosty
{

	class CollisionMap
	{


		//var
	public:


		//var
	private:
		unsigned int QuadVAO = 0;
		unsigned int m_GBuffer;
		unsigned int m_Texture;

		std::string m_VertexSrc;
		std::string m_FragmentSrc;
		uint32_t m_RendererID;

		const uint8_t m_Pix_Cord_Ratio = 10;

		static CollisionMap* s_Instance;
		std::shared_ptr <Camera> m_Camera;

		const uint16_t MAP_WITDH_PIXELS = 1920;
		const uint16_t MAP_HEIGHT_PIXELS = 1080;

		glm::mat4 m_View;
		glm::mat4 m_OrthoGraphic;

		float NearPlane = 0.1f;
		float FarPlane = 100.0f;

		glm::vec3 m_Pos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_LookAtVec = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 m_UpVec = glm::vec3(0.0f, 0.0f, -1.0f);


		friend class Application;
		//Func
	public:
		static CollisionMap* Get();
		virtual ~CollisionMap() {};

		void Initiate();

		std::shared_ptr<Camera>& GetCamera();



		//Func
	private:

		void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }
		void InitiateGBuffer();
		void InitiateShaders();
		void InitiateProgram();
		void RenderQuad();
	};


}

#endif
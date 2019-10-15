#ifndef COLLISIONMAP_HPP
#define COLLISIONMAP_HPP
#include"..\..\Core\Camera\Camera.hpp"

namespace Frosty
{

	class CollisionMap
	{


		struct CameraData
		{
			float FoV = glm::radians(60.0f);
			float AspRatio = 0.0f;
			float NearPlane = 0.1f;
			float FarPlane = 100.0f;


		};
		struct CameraTranslationData
		{
			glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 4.0f);
			glm::vec3 LookAtVec = glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 UpVec = glm::vec3(0.0f, 1.0f, 0.0f);

			float CamSpeed = 0.0f;
		};


		//var
	public:


		//var
	private:

		const uint8_t m_Pix_Cord_Ratio = 10;

		static CollisionMap* s_Instance;
		std::shared_ptr <Camera> m_Camera;

		const uint16_t MAP_WITDH_PIXELS = 1920;
		const uint16_t MAP_HEIGHT_PIXELS = 1080;

		glm::mat4 View;
		glm::mat4 Projection;
		glm::mat4 OrthoGraphic;

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






};


}

#endif
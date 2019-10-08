#ifndef CAMERA_HPP
#define CAMERA_HPP

namespace Frosty
{
	class Camera
	{
	public:
		struct CameraData
		{
			float FoV = glm::radians(60.0f);
			float AspRatio = 0.0f;
			float NearPlane = 0.1f;
			float FarPlane = 100.0f;

			glm::mat4 View;
			glm::mat4 Projection;
			glm::mat4 OrthoGraphic;
		};
		struct CameraTranslationData
		{
			glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 4.0f);
			glm::vec3 LookAtVec = glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 UpVec = glm::vec3(0.0f, 1.0f, 0.0f);

			float CamSpeed = 0.0f;
		};
		struct CameraRotationData
		{
			float LastX = 0.0f;
			float LastY = 0.0f;
			float Yaw = -90.0f;
			float Pitch = 0.0f;

			int MouseState = -1;
		};



	private:


		CameraData m_CameraData;
		CameraTranslationData m_CameraTranslationData;
		CameraRotationData m_CameraRotationData;
	public:
		Camera();
		virtual ~Camera();

		glm::mat4 GetView();
		glm::mat4 GetProjection();
		glm::mat4 GetOrthoGraphic();
		glm::mat4 GetViewProjection();	
		glm::vec3 GetPosition();

		CameraData& GetCameraData() { return m_CameraData; }
		CameraRotationData& GetCameraRotationData() { return m_CameraRotationData; }
		CameraTranslationData& GetCameraTranslationData() { return m_CameraTranslationData; }

		
		
		bool CameraPositionUpdate();
		void CameraRotationUpdate(double xpos, double ypos);
	};
}
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

namespace Frosty
{
	class Camera
	{
	private:
		struct CameraData
		{
			float FoV = glm::radians(90.0f);
			//float AspRatio = Application::Get().GetWindow().GetWidth() / Application::Get().GetWindow().GetHeight();
			float AspRatio = 1280 / 720;
			float NearPlane = 0.1f;
			float FarPlane = 100.0f;

			glm::mat4 View;
			glm::mat4 Projection;

			glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 2.0f);
			glm::vec3 LookAtVec = glm::vec3(0.0f, 0.75f, -1.0f);
			glm::vec3 UpVec = glm::vec3(0.0f, 1.0f, 0.0f);

			float CamSpeed = 0.0f;
			float DeltaTime = 0.0f;
			float CurrentFrame = 0.0f;
			float LastFrame = 0.0f;
			float Sensitivity = 0.0f;

			double MousePosX = 0;
			double MousePosY = 0;

			float LastX = 0.0f;
			float LastY = 0.0f;
			float Yaw = 0.0f;
			float Pitch = 0.0f;

			float XOffset = 0.0f;
			float YOffset = 0.0f;

			int MouseState = 0;
		};

		CameraData m_CameraData;		
	public:
		Camera();
		virtual ~Camera();

		glm::mat4 GetView();
		glm::mat4 GetProjection();
		glm::vec3 GetPos();

		void CameraPositionUpdate(GLFWwindow * window);
		void CameraRotationUpdate(GLFWwindow * window, double xpos, double ypos);
	};
}

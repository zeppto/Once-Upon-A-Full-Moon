#ifndef BASECAMERA_HPP
#define BASECAMERA_HPP

namespace Frosty
{
	class BaseCamera
	{
	public:
		BaseCamera();
		virtual ~BaseCamera();
		
		virtual void Initiate();
	protected:
		glm::mat4 GetView();
		glm::mat4 GetProjection();

		void CameraPositionUpdate();
		void CameraRotationUpdate(double xpos, double ypos);

		struct CameraData
		{
			float FoV = glm::radians(60.0f);


		};
	};
}
#endif

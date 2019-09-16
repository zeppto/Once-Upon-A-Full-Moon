#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frosty
{
	class TestCamera
	{
	private:
		float FoV;
		float aspRatio;
		float nearPlane;
		float farPlane;
				
		glm::mat4 view;
		glm::mat4 projection;	

		glm::vec3 camPos;
		glm::vec3 lookAtVec;
		glm::vec3 upVec;
	public:
		TestCamera();
		virtual ~TestCamera();

		glm::mat4 GetView()const;
		glm::mat4 GetProjection()const;
	};
}

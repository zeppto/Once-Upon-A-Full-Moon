#ifndef UIELEMENT_H
#define UIELEMENT_H
#include "Frosty/API/Transform.h"

namespace Frosty
{
	class UIElement
	{
	public:

	private:
		Transform m_Transform;
	public:
		UIElement();
		~UIElement();

		void SetAnchor(glm::vec2 anchor);
		void SetPosition(glm::vec2 pos);
		void SetScale(glm::vec2 scale);
		void SetRotate(glm::vec2 rot);

		glm::vec2 GetAnchor();
		glm::vec2 GetPosition();
		glm::vec2 GetScale();
		glm::vec2 GetRotate();

	private:

	};
}

#endif // !UIElement_H
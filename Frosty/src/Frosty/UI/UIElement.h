#ifndef UIELEMENT_H
#define UIELEMENT_H

namespace Frosty
{
	class UIElement
	{
	private:

	private:
		glm::vec2 m_position;

	public:
		UIElement();
		~UIElement();

	public:
		void setPosition(glm::vec2 pos);
		glm::vec2 getPosition() const;
	};
}

#endif // !UIElement_H
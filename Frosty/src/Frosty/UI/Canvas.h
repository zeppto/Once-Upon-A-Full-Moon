#ifndef CANVAS_H
#define CANVAS_H

namespace Frosty
{
	class Canvas
	{
	private:

		//std::vector<UIElement> element; //Button, Text, Texture, Icon, Sprite
		//Texture texture;
		
		glm::vec2 anchor;
		glm::vec2 pos;
		glm::vec2 size;
	public:
		Canvas(glm::vec2 size);
		~Canvas();

		bool createButton(glm::vec2 size = glm::vec2(50, 50));

		void setAnchor(glm::vec2 pivotPoint);
		void setPos(glm::vec2 pos);
		void setSize(glm::vec2 size);

	};
}


#endif // !CANVAS_H
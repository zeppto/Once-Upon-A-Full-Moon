#ifndef UIBOX_H
#define UIBOX_H

#include <string>

namespace Frosty {

	//Canvas
	class Canvas
	{
	private:
		
		//Canvas *m_containers[];
		//Buttons m_buttons[];
		glm::vec2 m_position;
		glm::vec2 m_scale;

	public:
		//Init Sprite
		Canvas();
		~Canvas();

		//Event
		bool createButton();
		//Get func
		glm::vec2 getPosition();
		glm::vec2 getScale();

		//Set func
		bool setTexture();
		void setPosition(glm::vec2 position);
		void setScale(glm::vec2 scale);

		//Draw
		void DrawAll();
	};

}
#endif // !UIBOX_H



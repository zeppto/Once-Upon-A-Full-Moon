#include "fypch.hpp"
#include "Canvas.h"

namespace Frosty
{

	Canvas::Canvas(glm::vec2 size = glm::vec2(500, 500))
	{
		this->anchor = glm::vec2(size.x / 2, size.y / 2);
		this->pos = glm::vec2(size.x / 2, size.y / 2);
		this->size = glm::vec2(size.x, size.y);
		//Test
	}

	Canvas::~Canvas()
	{
	}

	void Canvas::setAnchor(glm::vec2 anchor)
	{
		this->anchor = anchor;
	}

	void Canvas::setPos(glm::vec2 pos)
	{
		this->pos = pos;
	}

	void Canvas::setSize(glm::vec2 size)
	{
		this->size = size;
	}

}
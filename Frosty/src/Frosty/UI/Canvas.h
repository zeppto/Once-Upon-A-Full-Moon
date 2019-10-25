#ifndef CANVAS_H
#define CANVAS_H
#include "Frosty/UI/UIText.h"
#include "Frosty/UI/UIBar.hpp"

namespace Frosty
{
	class Canvas
	{
	public:
		//Pos_3, Uv_2, Normal_3, Color_4

	private:
		uint32_t m_MeshID;
		uint32_t m_TextureID;
		Transform m_Transform;
		//std::shared_ptr<VertexArray> m_vertexArray;
		//VertexArray test;

		//testing
		std::vector<Canvas> m_Canvas;
		std::vector<UIElement> m_Elements; //Button, Text, Texture, Icon, Sprite

		float** m_ElementVertices;
	public:
		Canvas(glm::vec2 size = glm::vec2(1, 1));
		~Canvas();

		void LoadTexture();
		void LoadMesh();

		void AddElement(UIElement element);

		void SetAnchor(glm::vec2 anchor);
		void SetPosition(glm::vec2 pos);
		void SetScale(glm::vec2 scale);
		void SetRotate(glm::vec2 rot);

		glm::vec2 GetAnchor();
		glm::vec2 GetPosition();
		glm::vec2 GetScale();
		glm::vec2 GetRotate();

		uint32_t GetTexture();
		uint32_t GetMesh();
		Transform& GetTransform();

		std::vector<std::vector<int32_t>> GetVertices() 
		{
			std::vector<std::vector<int32_t>> quads;
			for (int i = 0; i < m_Elements.size(); i++)
				quads.emplace_back(m_Elements[i].GetQuadPtrArray());
			return quads;
		};
	private:
	};
}
#endif

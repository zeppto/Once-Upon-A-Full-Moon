//#include "TestBoxModel.hpp"
//#include "Glad/glad.h"
#include "fypch.hpp"
//#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
//
//TestBoxModel::TestBoxModel(glm::vec3 color)
//{
//	this->color = color;
//}
//
//TestBoxModel::~TestBoxModel()
//{
//}
//
//void TestBoxModel::setUp()
//{
//	struct TriangleVertex
//	{
//		float x, y, z;
//		float r, g, b;
//		float u, v;
//	};
//	TriangleVertex triangleVertices[] =
//	{
//		//{ 0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
//		//{ 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 1.0f,	 1.0f, 1.0f},
//		//{-0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 0.0f,	0.0f, 1.0f},
//
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//
//		-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//
//		-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		 0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		 0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		-0.5f, -0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		-0.5f, -0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//
//		-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f,
//		 0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		 0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 1.0f, 0.0f,
//		-0.5f,  0.5f,  0.5f, color[0], color[1], color[2], 0.0f, 0.0f,
//		-0.5f,  0.5f, -0.5f, color[0], color[1], color[2], 0.0f, 1.0f
//
//	};
//
//	glGenVertexArrays(1, &this->testBoxVBO);
//	glBindVertexArray(this->testBoxVBO);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//	glEnableVertexAttribArray(2);
//
//	glGenBuffers(1, &this->testBoxVBO);
//	glBindBuffer(GL_ARRAY_BUFFER, this->testBoxVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices) * 3, triangleVertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
//	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 6));
//	glBindVertexArray(0);
//}
//
//void TestBoxModel::render()
//{
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	//glEnable(GL_DEPTH_TEST);
//
//	//glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
//
//	//for (int i = 0; i < 3; i++)
//	//{
//	//	glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
//	//	glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
//	//	glm::mat4 world = glm::mat4(1.0f);
//	//	//om i är 0 behandla som player
//	//	if (i == 0)
//	//	{
//	//		world = glm::translate(world, m_Camera->GetPos());
//	//		world = glm::translate(world, glm::vec3(0.0f, 0.0f, -3.0f));
//	//	}
//	//	else
//	//	{
//	//		world = glm::translate(world, glm::vec3(i, i, -2));
//	//	}
//	//	glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(world));
//
//
//	//	glBindVertexArray(this->m_testTriangleVBO);
//
//	//	glDrawArrays(GL_TRIANGLES, 0, 36);
//	//}
//
//	////glUniformMatrix4fv(1, 1, GL_FALSE, &m_Camera->GetView()[0][0]);
//	////glUniformMatrix4fv(2, 1, GL_FALSE, &m_Camera->GetProjection()[0][0]);
//
//	////glUseProgram(m_ShaderProgramVector.at(TEST_SHAPE));
//
//	////glBindVertexArray(this->m_testTriangleVBO);
//
//	//glDrawArrays(GL_TRIANGLES, 0, 36);
//}
//

#include "fypch.hpp"
#include "RenderCommand.hpp"
#include "Frosty/Core/Application.hpp"
#include <Glad/glad.h>

namespace Frosty
{
	void RenderCommand::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	
	//OLD DRAWINDEXED IS NOW Draw2D
	//void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	//{	
	//	/*if (vertexArray->GetIndexBuffer()->GetCount() != 0)
	//	{
	//		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	//		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	}*/
	//	/*else
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	}*/
	//
	//	// Temp ---------------------------------------------------------------------------
	//	Application& app = Application::Get();
	//	GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());
	//	// --------------------------------------------------------------------------------
	//	glEnable(GL_PROGRAM_POINT_SIZE);
	//	glPointSize(1.5f);
	//
	//	glDrawArrays(GL_POINTS, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//
	//	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	//	{
	//		//glDrawArrays(GL_POINTS, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	//	{
	//		glDrawArrays(GL_LINES, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//	}
	//	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	//	{
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	}
	//
	//	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	//glDrawArrays(GL_POINTS, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//	//glDrawArrays(GL_LINES, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//	//glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffer().front()->GetNrOfVertices());
	//	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//
	//	//int x = vertexArray->GetVertexBuffer()->GetSize();
	//	//int y = vertexArray->GetVertexBuffer()->GetNrOfVertices();
	//	//int y = vertexArray->GetVertexBuffer().front()->GetNrOfVertices();
	//	//int z = vertexArray->GetIndexBuffer()->GetCount();	
	//}

	void RenderCommand::Draw2D(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	//void RenderCommand::DrawParticles(const std::shared_ptr<VertexArray>& vertexArray, size_t count)
	//{
	//	glDrawArrays(GL_POINTS, 0, count);
	//}
}
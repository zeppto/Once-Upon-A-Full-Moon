#include "fypch.hpp"
#include "Glad/glad.h"
#include "RenderCommand.hpp"

namespace Frosty
{
	void RenderCommand::SetClearColor(const glm::vec4 & color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	}

	void RenderCommand::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (vertexArray->GetIndexBuffer()->GetCount() != 0)
		{
			glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);			
		}
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		//glDrawArrays(GL_PATCHES, 0, this->totalVertexSize);
		//int test = vertexArray->GetVertexBuffer().size();
		//int x = vertexArray->GetVertexBuffer().at(0)->GetSize();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//int test2 = GL_UNSIGNED_INT;
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawArrays(GL_TRIANGLES, 0, x);
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffer()->GetNrOfVertices());
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		int y = vertexArray->GetVertexBuffer()->GetNrOfVertices();
		int z = vertexArray->GetIndexBuffer()->GetCount();
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

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

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

	void RenderCommand::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RenderCommand::EnableBackfaceCulling()
	{
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}

	void RenderCommand::DisableBackfaceCulling()
	{
		glDisable(GL_CULL_FACE);
	}

	void RenderCommand::Draw2D(const std::shared_ptr<VertexArray>& vertexArray)
	{
		glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

	void RenderCommand::DrawParticles(const std::shared_ptr<VertexArray>& vertexArray, size_t count)
	{
		glDrawArrays(GL_POINTS, 0, (GLsizei)count);
	}
}
#include "fypch.hpp"
#include "Glad/glad.h"
#include "Renderer.hpp"

namespace Frosty
{	
	void Renderer::BeginScene()
	{
	}

	void Renderer::EndScene()
	{
	}
	
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		//vertexArray->Bind();
		//DrawIndexed(vertexArray);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}

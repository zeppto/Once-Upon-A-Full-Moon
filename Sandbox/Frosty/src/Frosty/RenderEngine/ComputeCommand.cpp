#include "ComputeCommand.hpp"
#include "fypch.hpp"
#include <Glad/glad.h>
#include "Frosty/Core/Application.hpp"

namespace Frosty
{
	void ComputeCommand::Init()
	{

	}

	void ComputeCommand::Send(size_t particleCount)
	{
		//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
		glDispatchCompute((GLuint)particleCount, 1, 1); //Particle count acts as number of work groups
		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
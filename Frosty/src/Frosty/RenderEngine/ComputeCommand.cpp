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
		glDispatchCompute(particleCount, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
}
#version 440 core
			
layout(location = 0) in vec3 vsInPos;
layout(location = 1) in vec2 vsInUV;
layout(location = 2) in vec3 vsInNorm;

out vec3 vsOutPos;
out vec2 vsOutUV;
out vec3 vsOutNorm;

void main()
{
	vsOutPos = vsInPos;
	vsOutUV = vsInUV;
	vsOutNorm = vsInNorm;
	gl_Position = vec4(vsInPos, 1.0f);
}
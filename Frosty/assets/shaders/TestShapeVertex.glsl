#version 440

layout(location = 0) in vec3 vsPosIn;
layout(location = 1) in vec3 vsColorIn;

layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 projection;
layout(location=3) uniform mat4 world;

out vec3 vsColorOut;

void main()
{
	vsColorOut = vsColorIn;
	gl_Position = projection * view * world * vec4(vsPosIn, 1.0f);
}
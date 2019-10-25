#version 440

layout(location = 0) in vec3 vsPosIn;
layout(location = 1) in vec3 vsColorIn;
layout(location = 2) in vec2 vsTexCoordIn;

layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 projection;
layout(location=3) uniform mat4 model;

out vec3 vsColorOut;
out vec2 vsTexCoordOut;

void main()
{
	vsColorOut = vsColorIn;
	vsTexCoordOut = vsTexCoordIn;
	gl_Position = projection * view * vec4(vsPosIn, 1.0f);
}

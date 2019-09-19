#version 440

layout(location = 0) in vec3 vsPosIn;
layout(location = 1) in vec3 vsColorIn;

layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 projection;

out vec3 worldPos;
out vec3 fragColor;

void main()
{
	worldPos =  mat3(projection * view) * vsPosIn;
	fragColor = vsColorIn;
	gl_Position = projection * view * vec4(vsPosIn, 1.0f);
}
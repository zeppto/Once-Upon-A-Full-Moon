#version 440

layout(location = 0) in vec3 vsPosIn;
layout(location = 1) in vec2 vsUvIn;
layout(location = 2) in vec3 vsNormalIn; // color/normal

layout(location=0) uniform mat4 model;
layout(location=1) uniform mat4 view;
layout(location=2) uniform mat4 projection;

layout(location = 0) out vec3 vsOutPos;
layout(location = 1) out vec2 vsOutUV;
layout(location = 2) out vec3 vsOutNormal; // color/normal


void main()
{
	vsOutPos =   vec3(model * vec4(vsPosIn, 1.0f));
	vsOutUV = vsUvIn;
	vsOutNormal = vsNormalIn;

	gl_Position = projection * view * model *vec4(vsPosIn, 1.0f);
}

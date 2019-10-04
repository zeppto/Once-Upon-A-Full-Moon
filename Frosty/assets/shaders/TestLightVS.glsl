#version 440

layout(location = 0) in vec3 vsInPos;
layout(location = 1) in vec4 vsInCol;

layout(location=2) uniform mat4 model;
layout(location=3) uniform mat4 u_ViewProjection;

out vec3 worldPos;
out vec4 fragColor;

void main()
{
//	worldPos =  vec3(model * vec4(vsPosIn, 1.0f));
	worldPos =  vsInPos;
	fragColor = vsInCol;

	gl_Position = u_ViewProjection * vec4(vsInPos, 1.0f);
}
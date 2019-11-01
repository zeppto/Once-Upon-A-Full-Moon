#version 440
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform mat4 viewMat;
uniform mat4 modelMat;

out vec4 outColor;

void main()
{
	outColor = color;
	gl_Position = (viewMat * modelMat) * vec4(position.xyz, 1.0f);
}
#version 440
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float size;

out vec4 outColor;
out float outSize;

void main()
{
	outColor = color;
	outSize = size;
	gl_Position = vec4(position.xyz, 1.0f);
}
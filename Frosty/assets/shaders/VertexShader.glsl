#version 440 core
			
layout(location = 0) in vec3 vsInPos;
layout(location = 1) in vec4 vsInCol;
			
uniform mat4 u_ViewProjection;

out vec3 vsOutPos;
out vec4 vsOutCol;

void main()
{
	vsOutPos = vsInPos;
	vsOutCol = vsInCol;
	gl_Position = u_ViewProjection * vec4(vsInPos, 1.0f);
}

#version 440
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 outColor[];
out vec4 finalColor;

out vec2 uvs;
uniform mat4 projectionMat;

const float size = 0.5;  //Temporary constant. Take in as uniform later.
const vec2 offsets[4] = { vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };

void main()
{
	for(int i = 0; i < 4; i++)
	{
		finalColor = outColor[0];
		uvs = offsets[i];
		vec4 pos = gl_in[0].gl_Position;
		pos.xy += size * ((offsets[i]) - vec2(size));
		gl_Position = projectionMat * pos;
		EmitVertex();
	}
	EndPrimitive();
}
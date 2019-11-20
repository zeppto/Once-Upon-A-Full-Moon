#version 440
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in float outSize[];
in vec4 outColor[];
out vec4 finalColor;

out vec2 uvs;
uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 modelMat;

const vec2 uvOffsets[4] = { vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
const vec2 offsets[4] = { vec2(-0.5f, -0.5f), vec2(-0.5f, 0.5f), vec2(0.5f, -0.5f), vec2(0.5f, 0.5f) };

void main()
{
	for(int i = 0; i < 4; i++)
	{
		finalColor = outColor[0];
		uvs = uvOffsets[i];
		vec2 pos = gl_in[0].gl_Position.xz + offsets[i] * outSize[0];
		gl_Position = (projectionMat * viewMat * modelMat) * vec4(pos.x, gl_in[0].gl_Position.y, pos.y, gl_in[0].gl_Position.w);
		EmitVertex();
	}
	EndPrimitive();
}
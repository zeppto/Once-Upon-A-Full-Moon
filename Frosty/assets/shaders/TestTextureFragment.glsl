#version 440

in vec3 vsColorOut;
in vec2 vsTexCoordOut;

out vec4 fsColorOut;

uniform sampler2D tex;

void main()
{
	fsColorOut = texture(tex, vsTexCoordOut) * vec4(vsColorOut, 1.0f);
}
#version 440
uniform sampler2D particleTexture;

in vec2 uvs;
in vec4 finalColor;
out vec4 finalPixel;

void main()
{
	finalPixel = texture(particleTexture, uvs) * finalColor;
}
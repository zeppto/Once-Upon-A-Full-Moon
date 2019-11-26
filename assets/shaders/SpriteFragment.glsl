#version 440
in vec2 texCoord;

layout(location = 0) out vec4 finalColor;

uniform sampler2D tex;
uniform vec4 color;

void main() {
	finalColor = vec4(texture(tex, texCoord) * color);
	// texture(texture, texCoord).rgb, 1.0f)
}
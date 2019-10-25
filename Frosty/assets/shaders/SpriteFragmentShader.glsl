#version 440 core
in vec3 vsOutPos;
in vec4 vsOutCol;
in vec2 vsOutUV;		

uniform sampler2D sprite_Texture;

layout(location = 0) out vec4 fsOutCol;

void main()
{
	//fsOutCol = vec4(0.8f, 0.2f, 0.3f, 1.0f);
	//fsOutCol = vec4(vsOutPos + 0.5f, 1.0f);
	vec3 tex = texture(sprite_Texture, vsOutUV).rgb;				
	fsOutCol = vec4(tex, 1.0f);
}
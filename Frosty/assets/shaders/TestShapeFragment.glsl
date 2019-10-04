#version 440

layout(binding = 0) uniform sampler2D tex;

layout(location = 0) in vec3 vsOutPos;
layout(location = 1) in vec2 vsOutUV;
layout(location = 2) in vec3 vsOutNormal; // color/normal

layout(location=3) uniform bool hasTexture;


layout(location= 4) uniform vec3 color;

out vec4 fsColorOut;

void main()
{
	vec4 diffTexture = vec4((texture(tex, vec2(vsOutUV.x, -vsOutUV.y)).xyz), 1.0);

	if(hasTexture)
	{
		fsColorOut = diffTexture + vec4(color,0);
	}
	else
	{
		fsColorOut = vec4(vsOutNormal, 1.0f);
	}

}

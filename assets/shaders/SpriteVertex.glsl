#version 440

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;

uniform mat4 projection;
uniform mat4 transform;

out vec2 texCoord;

void main(){
	gl_Position = projection * transform * vec4(a_Position.xy, 0.0f, 1.0f);
	texCoord = a_TextureCoords;
}
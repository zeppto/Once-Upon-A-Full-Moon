#version 440
layout (location = 0) in vec4 vertex; //vec2 position, vec2 tex

out vec2 texCoord;

uniform mat4 projection;

void main(){
	gl_Position = projection * vec4(vertex.xy, 0.0f, 1.0f);
	texCoord = vertex.zw;
}
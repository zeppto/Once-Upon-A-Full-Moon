#version 440 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TextureCoords;

uniform vec3 u_HealthBarSpace;

uniform vec3 u_Translate;
uniform vec3 u_Scale;

uniform mat4 u_ViewProjection;

uniform vec3 u_CameraRight_worldspace;
uniform vec3 u_CameraUp_worldspace;

out vec2 v_TextureCoords;

void main()
{
	v_TextureCoords = a_TextureCoords;

	gl_Position = u_ViewProjection * vec4(u_CameraRight_worldspace * a_Position.x * u_Scale.x + u_CameraUp_worldspace * a_Position.z * u_Scale.y + u_CameraRight_worldspace * u_Translate + u_HealthBarSpace, 1.0f);
                       
}
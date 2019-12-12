#version 440

uniform sampler2D u_DiffuseTexture;
in vec2 v_TextureCoords;

void main()
{	
	vec4 diffuseTexture = texture(u_DiffuseTexture, v_TextureCoords);
	if(diffuseTexture.a < 1)
	{
		gl_FragDepth = 1 ;
	}
	else
	{
		gl_FragDepth = gl_FragCoord.z ;
	}
}

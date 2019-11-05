#version 440
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in; //Threads

uniform float deltaTime;
uniform float maxLifetime; //Can be stored per particle instead of uniform for all

struct Particle
{
	vec4 position;
	vec4 color;
	vec4 direction;
	vec4 startPos;
	float lifetime;
	float speed;
};
layout (std430, binding = 0) buffer particles
{
	Particle particleData[];
};

void ResetParticle(uint ID)
{
	particleData[ID].position = particleData[ID].startPos;
}

void UpdateParticle(uint ID)
{
	particleData[ID].position.y -= 1.0f /*(particleData[ID].direction * particleData[ID].speed)*/ * deltaTime;
}

void main()
{
	uint index = gl_GlobalInvocationID.x;

	if(particleData[index].lifetime <= 0.0f)
	{
		particleData[index].lifetime = maxLifetime;
		ResetParticle(index);
	}

	particleData[index].lifetime -= 1.0f * deltaTime;
	UpdateParticle(index);
}
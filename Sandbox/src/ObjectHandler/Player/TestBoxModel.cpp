#include "fypch.hpp"
#include "TestBoxModel.hpp"
#include "Glad/glad.h"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))
	TestBoxModel::TestBoxModel(glm::vec3 color, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, int maxHp)
	{
		this->m_Color = color;
		//this->m_Color.g = 0.0f;

		this->m_Rotation = rotation;
		this->m_Pos = position;
		this->m_Scale = scale;

		this->m_TestBoxVBO = 0;
		this->m_ShaderProgram = 0;

		this->m_IsPlayer = false;
		this->m_ShouldRender = true;

		this->m_World = glm::mat4(1.0f);

		this->m_HitBoxCenter = glm::vec3(0.0f);
		this->m_HitBoxLength = glm::vec3(0.5f);

		//m_Min = m_World * glm::vec4(glm::vec3(-0.5f), 1);
		//m_Max = m_World * glm::vec4(glm::vec3(0.5f), 1);
		//for (int i = 0; i < 3; i++)
		//{
		//	if (m_Min[i] > m_Max[i])
		//	{
		//		float tempMax = m_Max[i];
		//		m_Max[i] = m_Min[i];
		//		m_Min[i] = tempMax;
		//	}
		//}
		this->m_MaxHp = maxHp;
		this->m_Hp = m_MaxHp;

		this->m_IsHit = false;
		this->m_timeLeftAfterHit = false;

		Update(position, rotation, scale);

	}

	TestBoxModel::~TestBoxModel()
	{
	}

	void TestBoxModel::SetUp()
	{
		struct TriangleVertex
		{
			float x, y, z;
			float r, g, b;
			float u, v;
		};
		TriangleVertex triangleVertices[] =
		{
			//{ 0.5f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	 1.0f, 0.0f },
			//{ 0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 1.0f,	 1.0f, 1.0f},
			//{-0.5f, -0.5f, 0.0f,	 1.0f, 0.0f, 0.0f,	0.0f, 1.0f},

			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, m_Color[0], m_Color[1], m_Color[2], 0.0f, 1.0f

		};

		glGenVertexArrays(1, &this->m_TestBoxVBO);
		glBindVertexArray(this->m_TestBoxVBO);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &this->m_TestBoxVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->m_TestBoxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices) * 3, triangleVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TriangleVertex), BUFFER_OFFSET(sizeof(float) * 6));
		glBindVertexArray(0);
	}

	void TestBoxModel::Render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glEnable(GL_DEPTH_TEST);
		if (m_ShouldRender)
		{

			glUseProgram(m_ShaderProgram);
			glBindVertexArray(this->m_TestBoxVBO);

			glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
			glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
			glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(m_World));
			glUniform3fv(4, 1, glm::value_ptr(m_Color));


			glBindVertexArray(this->m_TestBoxVBO);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}

	void TestBoxModel::SetShaderProgram(unsigned int shaderProgram)
	{
		this->m_ShaderProgram = shaderProgram;
	}

	void TestBoxModel::SetColor(glm::vec3 color)
	{
		this->m_Color = color;
	}

	void TestBoxModel::SetIsPlayer(bool isPlayer)
	{
		this->m_IsPlayer = isPlayer;
	}

	void TestBoxModel::SetShouldRender(bool shouldRender)
	{
		m_ShouldRender = shouldRender;
	}

	void TestBoxModel::SetPosition(glm::vec3 pos)
	{
		this->m_Pos = pos;
	}

	void TestBoxModel::SetHp(int hp)
	{
		m_Hp = hp;
	}

	void TestBoxModel::SetMaxHp(int maxHp)
	{
		m_MaxHp = maxHp;
	}

	int TestBoxModel::GetHp()
	{
		return m_Hp;
	}

	void TestBoxModel::IsAttacked(bool isAttacked)
	{
		if (isAttacked)
			m_Hp--;
	}

	bool TestBoxModel::Attacked()
	{
		if (m_IsPlayer)
		{
			return m_TheAttacks.playerCanAttack;
		}
		else
		{
			return false;
		}
	}

	glm::vec3 TestBoxModel::GetHitBoxLength()
	{
		//rotatin? scaler?
		return m_HitBoxLength * m_Scale;
	}

	glm::vec3 TestBoxModel::GetHitBoxCenter()
	{
		return m_HitBoxCenter + m_Pos;
	}

	glm::vec3 TestBoxModel::GetPos()
	{
		return m_Pos;
	}

	void TestBoxModel::Update(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		//pos += position;
		//world = glm::translate(world, pos);
		m_World = glm::mat4(1.0f);
		m_World = glm::translate(m_World, position);
		m_World = glm::rotate(m_World, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_World = glm::rotate(m_World, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_World = glm::rotate(m_World, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		m_World = glm::scale(m_World, scale);

		m_Pos = position;
		m_Rotation = rotation;
		m_Scale = scale;

		m_Color.g = 1 - ((float)m_Hp / m_MaxHp);

		if (m_Hp <= 0)
			m_ShouldRender = false;
	}
	void TestBoxModel::Update()
	{
		//if Player get movment
		if (m_IsPlayer)
		{
			m_Pos += m_PlayerController.PlayerControllerMovement();

			m_TheAttacks = m_PlayerController.PlayerControllerAttacks();
		}
		if (m_TheAttacks.enemyCanAttack)
			m_Hp--;

		m_World = glm::mat4(1.0f);
		m_World = glm::translate(m_World, m_Pos);
		m_World = glm::rotate(m_World, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		m_World = glm::rotate(m_World, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		m_World = glm::rotate(m_World, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		m_World = glm::scale(m_World, m_Scale);

		m_Color.g = 1 - ((float)m_Hp / m_MaxHp);

		if (m_Hp <= 0)
			m_ShouldRender = false;

	}



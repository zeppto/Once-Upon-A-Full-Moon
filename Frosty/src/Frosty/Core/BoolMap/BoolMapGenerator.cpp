#include<fypch.hpp>
#include"BoolMapGenerator.hpp"
#include "Glad/glad.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<stb_image_write.h>
#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

namespace Frosty
{
	//Lists
	BoolMapGenerator* BoolMapGenerator::s_Instance = nullptr;
	std::list<ModelBatch> BoolMapGenerator::s_ModelBatch = std::list<ModelBatch>();
	std::list<BoundBatch> BoolMapGenerator::s_BoundBatch = std::list<BoundBatch>();
	std::list<VABatch> BoolMapGenerator::s_VABatch = std::list<VABatch>();


	//RenderData
	glm::mat4 BoolMapGenerator::s_ViewOrtho(1.0f);
	unsigned int BoolMapGenerator::s_GBuffer = -1;
	unsigned int BoolMapGenerator::s_Texture = -1;
	unsigned int BoolMapGenerator::s_RenderProgramID = -1;

	BoolMapGenerator::GeneratorSettings BoolMapGenerator::s_Settings = BoolMapGenerator::GeneratorSettings();
	
	
	
	BoolMapGenerator* BoolMapGenerator::Get()
	{
		if (s_Instance == nullptr)
		{
			Init();
		}
		return s_Instance;
	}

	void BoolMapGenerator::InitiateRenderData()
	{

		s_Settings.Pos = glm::vec3((s_Settings.Width) / 2.0f, 100.0f, ((float)s_Settings.Height) / 2.0f);

		glm::vec3 tempDir = s_Settings.Pos;
		tempDir[1] = 0.0f;
		glm::mat4 tempView = glm::lookAt(s_Settings.Pos, tempDir, s_Settings.UpVec);
		glm::vec4 OrthoVec((float)s_Settings.Width / -2.0f, (float)s_Settings.Width / 2.0f, (float)s_Settings.Height / 2.0f, (float)s_Settings.Height / -2.0f);
		glm::mat4 tempOrtho = glm::ortho(OrthoVec[0], OrthoVec[1], OrthoVec[2], OrthoVec[3], 1.0f, 200.0f);

		s_ViewOrtho = tempOrtho * tempView;

		InitiateGBuffer();
	}

	void BoolMapGenerator::Init()
	{
		if (s_Instance != nullptr)
		{
			delete s_Instance;
		}
		s_Instance = new BoolMapGenerator;
		InitiateProgram();
	}

	void BoolMapGenerator::InitCheck()
	{
		if (s_Instance == nullptr) {
			Init();
		}
	}

	void BoolMapGenerator::InitiateGBuffer()
	{
		glGenFramebuffers(1, &s_GBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, s_GBuffer);
		glViewport(0, 0, (s_Settings.Width * s_Settings.Pix_Cord_Ratio), (s_Settings.Height * s_Settings.Pix_Cord_Ratio));
		glGenTextures(1, &s_Texture);
		glBindTexture(GL_TEXTURE_2D, s_Texture);
		//glGenerateMipmap(GL_TEXTURE_2D);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, (s_Settings.Width * s_Settings.Pix_Cord_Ratio), (s_Settings.Height * s_Settings.Pix_Cord_Ratio), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_Texture, 0);

		unsigned int attachments =  (unsigned int)GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &attachments);

		char buff[1024];
		memset(buff, 0, 1024);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(s_RenderProgramID, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Collision Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);
		}
	}


	std::shared_ptr<BoolMap> BoolMapGenerator::RenderMap()
	{
		//temp
	//	glDeleteTextures(1, &s_Texture);

		InitiateRenderData();

		uint16_t TmpHeight = s_Settings.Height * s_Settings.Pix_Cord_Ratio;
		uint16_t TmpWidth = s_Settings.Width * s_Settings.Pix_Cord_Ratio;

		unsigned int VertID;
		unsigned int IndID;

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(s_RenderProgramID);

		GLint locationVO = glGetUniformLocation(s_RenderProgramID, "u_ViewOrtho");
		GLint locationMM = glGetUniformLocation(s_RenderProgramID, "u_ModelMat");
		glUniformMatrix4fv(locationVO, 1, GL_FALSE, &s_ViewOrtho[0][0]);

		//Render

		//ModelBatch
		std::list<ModelBatch>::iterator ModelBatchIt = s_ModelBatch.begin();
		while (ModelBatchIt != s_ModelBatch.end())
		{
			glBindVertexArray(0);
			glGenVertexArrays(1, &VertID);

			glBindVertexArray(VertID);
			glBindBuffer(GL_ARRAY_BUFFER, VertID);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)* ModelBatchIt->Verticies.size(), &ModelBatchIt->Verticies[0], GL_STATIC_DRAW);


			glGenBuffers(1, &IndID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ModelBatchIt->Indices.size(), &ModelBatchIt->Indices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);



			std::list<glm::mat4>::iterator PosIt = ModelBatchIt->Transforms.begin();
			while (PosIt != ModelBatchIt->Transforms.end())
			{
				//glBindVertexArray(RenderModelID);
				glUniformMatrix4fv(locationMM, 1, GL_FALSE, &(*PosIt)[0][0]);
				//glDrawArrays(GL_TRIANGLES, 0, 3);

				glDrawElements(GL_TRIANGLES, (int)ModelBatchIt->Indices.size(), GL_UNSIGNED_INT, 0);
				PosIt++;
			}
			ModelBatchIt++;

			//glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(0);
			glDeleteBuffers(1, &IndID);
		
			glDeleteVertexArrays(1, &VertID);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		//BoundBatch
		std::list<BoundBatch>::iterator BoundBatchIt = s_BoundBatch.begin();
		while (BoundBatchIt != s_BoundBatch.end())
		{
			glBindVertexArray(BoundBatchIt->VertexArrayID);
			std::list<glm::mat4>::iterator PosIt = BoundBatchIt->Transforms.begin();
			while (PosIt != BoundBatchIt->Transforms.end())
			{
				glUniformMatrix4fv(locationMM, 1, GL_FALSE, &(*PosIt)[0][0]);
				glDrawElements(GL_TRIANGLES, BoundBatchIt->NrOfIndices, GL_UNSIGNED_INT, 0);
				PosIt++;
			}
			BoundBatchIt++;
		}

		//Need to be tested
		//VABatch
		std::list<VABatch>::iterator VABatchIt = s_VABatch.begin();
		while (VABatchIt != s_VABatch.end())
		{
			VABatchIt->VertexArrayObj->Bind();
			std::list<glm::mat4>::iterator PosIt = VABatchIt->Transforms.begin();
			while (PosIt != VABatchIt->Transforms.end())
			{
				glUniformMatrix4fv(locationMM, 1, GL_FALSE, &(*PosIt)[0][0]);
				glDrawElements(GL_TRIANGLES, VABatchIt->VertexArrayObj->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
				PosIt++;
			}
			VABatchIt->VertexArrayObj->Unbind();
			VABatchIt++;
		}






		const size_t bytesPerPixel = 3;	// RGB
		const size_t imageSizeInBytes = bytesPerPixel * size_t(TmpWidth) * size_t(TmpHeight);
		//int texSize = TmpWidth * TmpHeight * 3 * 4;
		BYTE* pixels = static_cast<BYTE*>(malloc(imageSizeInBytes));
		//uint8_t* tempintPtr = FY_NEW uint8_t[texSize];
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, TmpWidth, TmpHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		std::string tempstr(("Test.jpg"));
		int check = stbi_write_jpg(tempstr.c_str(), TmpWidth, TmpHeight, 3, pixels, 100);

		free(pixels);
		//delete tempintPtr;







		int texSize = (unsigned int)TmpWidth * (unsigned int)TmpHeight;

		float* tempFloatPtr = FY_NEW float[texSize];
		glReadPixels(0, 0, TmpWidth, TmpHeight, GL_RED, GL_FLOAT, &tempFloatPtr[0]);

		int bitSize = (int)std::ceil((texSize / 64.0f));
		std::shared_ptr<uint64_t[]> bitMap(FY_NEW uint64_t[bitSize]);

		//temp
		//boolmap
		//std::shared_ptr<bool[]> tmpMap(FY_NEW bool[texSize]);

		//for (unsigned int i = 0; i < texSize; i++)
		//{
		//	//temp
		//	float xx = tempFloatPtr[i];

		//	if (tempFloatPtr[i] > 0.001)
		//	{
		//		tmpMap[i] = true;
		//	}
		//	else
		//	{
		//		tmpMap[i] = false;
		//	}



		//}


		int32_t bitmapCount = -1;
		//bitmap
		uint64_t currentInt = 0;

		for (unsigned int i = 0; i < unsigned int(texSize); i++)
		{
			if (i % 64 == 0 || bitmapCount == -1 || i == texSize-1)
			{
				if (bitmapCount != -1)
				{
					bitMap[bitmapCount] = currentInt;
					currentInt = 0;
				}
				bitmapCount++;
			}

			//if (tempFloatPtr[i] > 0.5)
			//{
			//	int j = 0;
			//}

			currentInt = (tempFloatPtr[i] > 0.001) ? (currentInt << 1) + 1 : currentInt << 1;
		}


		delete[]tempFloatPtr;


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteFramebuffers(1, &s_GBuffer);
	//	glDeleteTextures(1, &s_Texture);
		s_VABatch.erase(s_VABatch.begin(), s_VABatch.end());
		s_ModelBatch.erase(s_ModelBatch.begin(), s_ModelBatch.end());
		s_BoundBatch.erase(s_BoundBatch.begin(), s_BoundBatch.end());

//		return std::shared_ptr<BoolMap>(FY_NEW BoolMap(TmpWidth, TmpHeight, s_Settings.Pix_Cord_Ratio, tmpMap, bitMap));
		return std::shared_ptr<BoolMap>(FY_NEW BoolMap(TmpWidth, TmpHeight, s_Settings.Pix_Cord_Ratio, bitMap, bitmapCount));
	}


	void BoolMapGenerator::InitiateProgram()
	{
		char buff[1024];
		memset(buff, 0, 1024);
		GLint compileResult = 0;

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);

		const char* shaderTextPtr = VERTEXSRC.c_str();

		glShaderSource(vs, 1, &shaderTextPtr, nullptr);
		glCompileShader(vs);
		glGetShaderiv(vs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			glGetShaderInfoLog(vs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator: Failed to compile Vertex Shader, GLFW Error ({0}) \n)", buff);
		}

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

		shaderTextPtr = FRAGMENTSRC.c_str();

		glShaderSource(fs, 1, &shaderTextPtr, nullptr);
		glCompileShader(fs);
		compileResult = GL_FALSE;
		glGetShaderiv(fs, GL_COMPILE_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetShaderInfoLog(fs, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ERROR("Collision Map Generator: Failed to compile Fragment Shader, GLFW Error ({0}) \n", buff);
		}

		s_RenderProgramID = glCreateProgram();
		//	GLuint tempProgram = RenderProgramID;
		glAttachShader(s_RenderProgramID, vs);

		glAttachShader(s_RenderProgramID, fs);
		glLinkProgram(s_RenderProgramID);

		compileResult = GL_FALSE;
		glGetProgramiv(s_RenderProgramID, GL_LINK_STATUS, &compileResult);
		if (compileResult == GL_FALSE)
		{
			memset(buff, 0, 1024);
			glGetProgramInfoLog(s_RenderProgramID, 1024, nullptr, buff);
			OutputDebugStringA(buff);
			FY_CORE_ASSERT("Collision Map Generator: Failed to Link program, GLFW Error ({0}) \n)", buff);
		}
		else
		{
			FY_CORE_INFO("Success Generation of BoolMap Program)");
		}


		glDetachShader(s_RenderProgramID, vs);
		glDetachShader(s_RenderProgramID, fs);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}


}
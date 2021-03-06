#ifndef BOOLMAPGENERATOR_HPP
#define BOOLMAPGENERATOR_HPP
#include<Frosty/API/AssetManager/AssetFiles/BoolMap.hpp>
#include<Frosty/RenderEngine/VertexArray.hpp>
#define DEFAULT_BOOLMAP_WIDTH 300
#define DEFAULT_BOOLMAP_HEIGHT 300
#define DEFAULT_BOOLMAP_PIXELCOORD_RATIO 10

#define STB_IMAGE_WRITE_IMPLEMENTATION
//Change to MCS
namespace Frosty
{
	

	//If needed memory convert list mat4 to pointers 
	struct ModelBatch
	{
		std::vector<float> Verticies;
		std::vector<unsigned int> Indices;
		std::list<glm::mat4> Transforms;
	};

	struct BoundBatch
	{
		unsigned int VertexArrayID;
		unsigned int NrOfIndices;
		std::list<glm::mat4> Transforms;
	};

	struct VABatch
	{	
		//Pointer?
		std::shared_ptr<VertexArray> VertexArrayObj;
		std::list<glm::mat4> Transforms;
	};

	//class VertexArray;

	class BoolMapGenerator
	{

		
		//var
	public:


	private:
		inline static const std::string VERTEXSRC = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			
			layout(location = 44) uniform mat4 u_ViewOrtho;
			layout(location = 33) uniform mat4 u_ModelMat;
			
			void main()
			{				
				gl_Position = u_ViewOrtho *u_ModelMat * vec4(vsInPos, 1.0f);
			}
		)";
		inline static const std::string FRAGMENTSRC = R"(
			#version 440 core

			layout(location = 0) out vec4 fsOutCol; 
			
			void main()
			{
				fsOutCol = vec4( 1.0f, 0.0f, 0.0f, 1.0f ); 
			}
		)";

		//public?
		struct GeneratorSettings
		{

			float NearPlane = 0.1f;
			float FarPlane = 100.0f;

			uint8_t Pix_Cord_Ratio = DEFAULT_BOOLMAP_PIXELCOORD_RATIO;

			glm::vec3 Pos = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 LookAtVec = glm::vec3(0.0f, -1.0f, 0.0f);
			glm::vec3 UpVec = glm::vec3(0.0f, 0.0f, -1.0f);

			uint16_t Width = DEFAULT_BOOLMAP_WIDTH;
			uint16_t Height = DEFAULT_BOOLMAP_HEIGHT;
		};



		static BoolMapGenerator* s_Instance;
		friend class Application;

		//BatchLists
		static std::list<ModelBatch> s_ModelBatch;
		static std::list<BoundBatch> s_BoundBatch;
		static std::list<VABatch> s_VABatch;

		static GeneratorSettings s_Settings;

		//RenderData
		static glm::mat4 s_ViewOrtho;

		static unsigned int s_GBuffer;
		static unsigned int s_Texture;
		static unsigned int s_RenderProgramID;
		
		//Func
	public:
		static BoolMapGenerator* Get();
		virtual ~BoolMapGenerator() {};

		inline static void AddBatch(const ModelBatch& Batch) { InitCheck();  s_ModelBatch.push_back(Batch); }
		inline static void AddBatch(const BoundBatch& Batch) { InitCheck();  s_BoundBatch.push_back(Batch); }
		inline static void AddBatch(const VABatch& Batch) { InitCheck();  s_VABatch.push_back(Batch); }

		inline static std::shared_ptr<BoolMap> RenderBoolMap() { InitCheck(); return RenderMap(); }

		inline static  const unsigned int& GetTextureID() { return s_Texture; }


		//Func
	private:
		// 1 calls
		static void Init();
		static void InitCheck();
		static void InitiateProgram();
		static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }

		static void InitiateRenderData();
		static void InitiateGBuffer();
		static std::shared_ptr<BoolMap> RenderMap();
	};


}

#endif
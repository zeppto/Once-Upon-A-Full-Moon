#ifndef BOOLMAPGENERATOR_HPP
#define BOOLMAPGENERATOR_HPP
#include"..\..\Core\Camera\Camera.hpp"
#include"BoolMap.hpp"

#define DEFAULT_BOOLMAP_WIDTH 128
#define DEFAULT_BOOLMAP_HEIGHT 72
#define DEFAULT_BOOLMAP_PIXELCOORD_RATIO 10

namespace Frosty
{



	struct ModelBatch
	{
		std::vector<float> Verticies;
		std::vector<float> Indices;
		std::list<glm::mat4> Transforms;
	};



	class BoolMapGenerator
	{


		//var
	public:


		//var
	private:
		inline static const std::string VERTEXSRC = R"(
			#version 440 core
			
			layout(location = 0) in vec3 vsInPos;
			
			layout(location = 44) uniform mat4 u_ViewOrtho;
			layout(location = 33) uniform mat4 u_ModelMat;
			
			void main()
			{				
				gl_Position = u_ViewOrtho * u_ModelMat * vec4(vsInPos, 1.0f);
			}
		)";
		inline static const std::string FRAGMENTSRC = R"(
			#version 440 core

			layout(location = 0) out vec4 fsOutCol; 
			
			void main()
			{
				fsOutCol = vec4( 0.0f, 5.0f, 0.0f, 1.0f ); 
			}
		)";


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


		//temp
		static unsigned int s_QuadVAO;
		static std::shared_ptr <Camera> m_Camera;
		static unsigned int m_VertexArray;
		//


		static BoolMapGenerator* s_Instance;
		friend class Application;

		//outside Info
		static std::list<ModelBatch> s_ModelList;
		static GeneratorSettings s_Settings;

		//RenderData

		static glm::mat4 s_ViewOrtho;
		//static glm::mat4 s_Ortho;

		static unsigned int s_GBuffer;
		static unsigned int s_Texture;
		static unsigned int s_RenderProgramID;


		//Func
	public:
		static BoolMapGenerator* Get();
		virtual ~BoolMapGenerator() {};

		inline static void AddToRenderList(const ModelBatch& Batch) { InitCheck();  s_ModelList.push_back(Batch); }

		inline static std::shared_ptr<BoolMap>& RenderBoolMap() { InitCheck(); return RenderMap(); }

		//temp
		static std::shared_ptr<Camera>& GetCamera();



		//Func
	private:
		// 1 calls
		static void Init();
		static void InitiateProgram();
		static void Delete() { if (s_Instance != nullptr) { delete s_Instance; } }


		static void InitCheck();

		
		static void InitiateRenderData();
		static void InitiateGBuffer();
		static std::shared_ptr<BoolMap>& RenderMap(); // start here
	};


}

#endif
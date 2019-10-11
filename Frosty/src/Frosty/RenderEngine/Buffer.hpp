#ifndef BUFFER_HPP
#define BUFFER_HPP

namespace Frosty
{
	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:
			break;
		case ShaderDataType::Float:
			return 4;
			break;
		case ShaderDataType::Float2:
			return 4 * 2;
			break;
		case ShaderDataType::Float3:
			return 4 * 3;
			break;
		case ShaderDataType::Float4:
			return 4 * 4;
			break;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
			break;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
			break;
		case ShaderDataType::Int:
			return 4;
			break;
		case ShaderDataType::Int2:
			return 4 * 2;
			break;
		case ShaderDataType::Int3:
			return 4 * 3;
			break;
		case ShaderDataType::Int4:
			return 4 * 4;
			break;
		case ShaderDataType::Bool:
			return 1;
			break;		
		}

		FY_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetElementSize() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:  return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Mat3:	 return 3 * 3;
				case ShaderDataType::Mat4:	 return 4 * 4;
				case ShaderDataType::Int:	 return 1;				
				case ShaderDataType::Int2:	 return 2;				
				case ShaderDataType::Int3:	 return 3;				
				case ShaderDataType::Int4:	 return 4;				
				case ShaderDataType::Bool:	 return 1;				
			}

			FY_CORE_ASSERT(false, "Unknown ShaderDataType");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& element)
			: m_Element(element)
		{
			CalculateOffsetAndStride();
		}

		inline uint32_t GetStride()const { return m_Stride; }
		inline const std::vector<BufferElement>& GetElements()const { return m_Element; }

		std::vector<BufferElement>::iterator begin() { return m_Element.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Element.end(); }

	private:
		void CalculateOffsetAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;

			for (auto& element : m_Element)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Element;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(const void* vertices, uint32_t size);
		VertexBuffer();
		virtual ~VertexBuffer();

		void Bind()const;
		void Unbind()const;

		void SetNrOfVertices(uint32_t count);
		uint32_t GetNrOfVertices()const;
		uint32_t GetSize()const;

		BufferLayout GetLayout()const { return m_Layout; }
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

		static VertexBuffer* Create(const void* vertices, uint32_t size);
		static VertexBuffer* Create();

		void SetData(float* vertices, uint32_t size, uint32_t type);

	private:
		uint32_t m_RendererID;
		uint32_t m_Size;
		uint32_t m_NrOfVertices;
		BufferLayout m_Layout;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(const void* indices, uint32_t count);
		virtual ~IndexBuffer();

		void Bind()const;
		void Unbind()const;

		uint32_t GetCount()const;

		static IndexBuffer* Create(const void* indices, uint32_t count);
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;		
	};
}
#endif

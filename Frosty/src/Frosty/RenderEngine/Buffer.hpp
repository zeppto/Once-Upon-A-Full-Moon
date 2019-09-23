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
		int3,
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
		case ShaderDataType::int3:
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

		BufferElement(const std::string& name, ShaderDataType type)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0)
		{
		}
	};
	class VertexBuffer
	{
	public:
		VertexBuffer(float* vertices, uint32_t size);
		virtual ~VertexBuffer();

		void Bind()const;
		void Unbind()const;

		BufferLayout GetLayout()const { return m_Layout; }
		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

		static VertexBuffer* Create(float* vertices, uint32_t size);
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};
	class IndexBuffer
	{
	public:
		IndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~IndexBuffer();

		void Bind()const;
		void Unbind()const;

		uint32_t GetSize()const;

		static IndexBuffer* Create(uint32_t* indices, uint32_t size);
	private:
		uint32_t m_RendererID;
		uint32_t m_Size;		
	};
	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement>& element) 
			: m_Element(element)
		{
			CalculateOffsetAndStride();
		}

		inline const std::vector<BufferElement>& GetElements()const { return m_Element; }
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
}
#endif // !BUFFER_HPP

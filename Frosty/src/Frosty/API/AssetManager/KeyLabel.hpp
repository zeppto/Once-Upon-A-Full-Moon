#ifndef KEYHEAD_HPP
#define KEYHEAD_HPP
#include"AssetMetaData.hpp"
#include"BaseKey.hpp"

namespace Frosty
{
	template<class T>
	class KeyLabel : public BaseKey
	{

		//Variables
	public:


		//Variables
	private:

		 AssetMetaData<T>* m_MetaData_ptr;
		 std::string m_Label;

		//Functions
	public:

		KeyLabel() : BaseKey(), m_MetaData_ptr(nullptr) {}
		KeyLabel(T& data) : BaseKey(), m_MetaData_ptr(&data) {}
		KeyLabel(KeyLabel& other) : BaseKey(other) , m_MetaData_ptr(other.m_MetaData_ptr) {}
		virtual ~KeyLabel() {}

		bool operator ==(const KeyLabel& other) { return (m_MetaData_ptr == other.m_MetaData_ptr);}
		
		KeyLabel& operator = (const KeyLabel& other) 
		{
			m_MetaData_ptr = other.m_MetaData_ptr;
			m_Label = other.m_Label;
			return *this; 
		}

		void SetLabel(const std::string& Label) { m_Label = Label; }
		const std::string& GetLabel() { return m_Label; }

		T& GetKeyData() { return *m_MetaData_ptr->GetData(); }

		void SetKeyData(AssetMetaData<T>& data) {m_MetaData_ptr = &data;}

		const uint32_t& GetKeyID() { return m_Key_ID; }
		const uint32_t& GetKeyCount() { return s_NrOf_Keys; }

		//Functions
	private:
	};
}
#endif // !KEYHEAD_HPP

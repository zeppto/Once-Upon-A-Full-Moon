#ifndef KEYHEAD_HPP
#define KEYHEAD_HPP

#include"KeyLabel.hpp"


namespace Frosty
{
	template<class T>
	class KeyLabel : public BaseKey
	{

		//Variables
	public:


		//Variables
	private:

		 T* m_Data_ptr;
		 std::string m_Label;

		//Functions
	public:

		KeyLabel() : BaseKey(), m_Data_ptr(nullptr) {}
		KeyLabel(T& data) : BaseKey(), m_Data_ptr(&data) {}
		KeyLabel(KeyLabel& other) : BaseKey(other) , m_Data_ptr(other.m_Data_ptr) {}
		virtual ~KeyLabel() {}

		inline	bool operator ==(const KeyLabel& other) { return (m_Data_ptr == other.m_Data_ptr);}
		
		inline KeyLabel& operator = (const KeyLabel& other) 
		{
			m_Data_ptr = other.m_Data_ptr;
			m_Label = other.m_Label;
			return *this; 
		}

		inline void SetLabel(const std::string& Label) { m_Label = Label; }
		inline const std::string& GetLabel() const { return m_Label; }

		inline T& GetKeyData() { return *m_Data_ptr->GetData(); }

		void SetKeyData(T& data) {m_Data_ptr = &data;}

		const uint32_t& GetKeyID() { return m_Key_ID; }

		const uint32_t& GetKeyCount() { return s_NrOf_Keys; }



		//Functions
	private:

	};




}

#endif // !KEYHEAD_HPP

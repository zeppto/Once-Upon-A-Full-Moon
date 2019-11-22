#ifndef BASEKEY_HPP
#define BASE_KEY


namespace Frosty
{

	class BaseKey
	{

		//Variables
	public:



		//Variables
	private:
		static uint32_t s_Unique_ID;
		static uint32_t s_NrOf_Keys;

		//Variables
	protected:
		uint32_t m_Key_ID;
		uint32_t m_Key_ReferenceID;

		//Functions
	public:

		BaseKey();
		BaseKey(BaseKey& other);
		virtual ~BaseKey();

		void SetRefKey(uint32_t ref_id) { m_Key_ReferenceID = ref_id; };
		const uint32_t GetRefKeyId() {return m_Key_ReferenceID;};
		BaseKey& operator = (BaseKey& other);
		bool operator ==(BaseKey& other) { return(m_Key_ReferenceID == other.m_Key_ReferenceID); }


		virtual const uint32_t& GetKeyID() = 0;


		//Functions
	private:



	};

}

#endif // !BASEKEY_HPP

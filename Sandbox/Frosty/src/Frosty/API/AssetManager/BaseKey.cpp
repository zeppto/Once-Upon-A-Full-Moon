#include "fypch.hpp"
#include "BaseKey.hpp"

namespace Frosty
{

	uint32_t BaseKey::s_NrOf_Keys = 0;
	uint32_t BaseKey::s_Unique_ID = 0;

	Frosty::BaseKey::BaseKey()
	{
		m_Key_ID = s_Unique_ID++;
		s_NrOf_Keys++;
	}

	BaseKey::BaseKey(BaseKey& other)
	{
		if (this != &other)
		{
			m_Key_ID = s_Unique_ID++;
			s_NrOf_Keys++;
		}
		m_Key_ReferenceID = other.m_Key_ReferenceID;
	}

	BaseKey::~BaseKey()
	{
		s_NrOf_Keys--;
	}

	BaseKey& BaseKey::operator=(BaseKey& other)
	{
		if (this != &other)
		{
			m_Key_ID = s_Unique_ID++;
			s_NrOf_Keys++;
		}
		return *this;
	}





}

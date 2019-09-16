#include "fypch.hpp"
#include "Assetmanager.hpp"

namespace Frosty
{


	Assetmanager* Frosty::Assetmanager::getAssetmanager()
	{
			if (!m_Instance)
				m_Instance = new Assetmanager;
			return m_Instance;
	}

	Frosty::Assetmanager::~Assetmanager()
	{
		delete m_Instance;
	}



}
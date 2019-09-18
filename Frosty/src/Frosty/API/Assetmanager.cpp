#include "fypch.hpp"
#include "Assetmanager.hpp"

namespace Frosty
{
	Assetmanager* Assetmanager::m_Instance = nullptr;

	Assetmanager* Frosty::Assetmanager::GetAssetmanager()
	{
		if (!m_Instance)
		{
			m_Instance = new Assetmanager;

		}
			return m_Instance;
	}

	bool Assetmanager::LoadFile(std::string FilePath)
	{

		


		return false;
	}

	bool Assetmanager::LoadFile(std::string FilePath, const std::string& set_Prefabkey)
	{
		return false;
	}

	Frosty::Assetmanager::~Assetmanager()
	{
		if (m_Instance != nullptr)
		{
		delete m_Instance;
		}
	}



}
#ifndef ASSETMETADATA_HPP
#define ASSETMETADATA_HPP

namespace Frosty
{

	template<class T>
	class AssetMetaData
	{
		//Variables
	public:



		//Variables
	private:

		static int32_t s_Asset_ID;
		static int32_t s_Asset_Counter;

		//Can be changed to another type of pointer (was planned for a type of "Backup" for loading)
		T** const m_data_ptr = FY_NEW T*;

		int32_t m_Asset_ID;
		int32_t m_Asset_Container_Slot;

		std::string m_FileName;
		std::string m_FilePath;

		unsigned int m_GetCounter = 0; //For Current Object
		unsigned long int m_TotalGetCounter = 0;

		//Functions
	public:

		AssetMetaData();
		AssetMetaData(const AssetMetaData& other);
		virtual ~AssetMetaData();
	//	AssetMetaData(T& refData, std::string FileName);

		bool operator ==(const AssetMetaData& other) { return (m_FileName == other.m_FileName); }

		//Data Getters
		T* GetData();
		bool GetData(T* inPtr);

		const std::string& GetFileName() const { return m_FileName; }
		const std::string& GetFilePath() const { return m_FilePath; }
		const int32_t& GetAssetContainerSlot() const { return m_Asset_Container_Slot; }

		//Asset Id
		const int32_t& GetAssetId() const { return m_AssetID; }
		void SetNewAssetID() { m_AssetID = s_Asset_ID++; }

		//Counters
		unsigned int& GetTimesVisited() const { return m_GetCounter; }
		const unsigned long int& GetTotalTimesVisited() const { return m_TotalGetCounter; }


		void SetMetaDataToNull();

		void SetRefData(T & newRefData) { *m_data_ptr = &newRefData; }
		void SetFileName(const std::string & FileName) { m_FileName = FileName; }
		void SetFilePath(const std::string & FilePath) { m_FilePath = FilePath; }
		void SetContainerSlot(const int32_t& NewSlot) { m_Asset_Container_Slot = NewSlot; }
		void SetAllRefData(T & newRefData, const std::string & FileName, const std::string & FilePath, int32_t ContainerSlot);

		//Functions
	private:


	};

#endif // !ASSETMETADATA_HPP


	template<class T>
	inline AssetMetaData<T>::AssetMetaData()
	{
		*m_data_ptr = nullptr;
		m_Asset_ID = s_Asset_ID++;
		m_FileName = "";
		m_FilePath = "";
		s_Asset_Counter++;
		m_Asset_Container_Slot = -1;
	}

	//template<class T>
	//inline AssetMetaData<T>::AssetMetaData(T& refData, std::string FileName)
	//{
	//	*m_data_ptr = &refData;
	//	m_AssetID = s_Asset_ID++;
	//	m_FileName = FileName;
	//	m_FilePath = other.m_FilePath;
	//	s_Asset_Counter++;
	//}

	template<class T>
	inline AssetMetaData<T>::AssetMetaData(const AssetMetaData& other)
	{
		if (this != &other) {
			s_Asset_Counter++;
		}
		m_Asset_ID = s_Asset_ID++;
		*m_data_ptr = *other.m_data_ptr;
		m_FilePath = other.m_FilePath;
		m_FileName = other.m_FileName;
		m_Asset_Container_Slot = other.m_Asset_Container_Slot;

	}

	template<class T>
	inline AssetMetaData<T>::~AssetMetaData()
	{
		s_Asset_Counter--;
		delete m_data_ptr;
		*m_data_ptr = nullptr;
	}

	template<class T>
	inline T* AssetMetaData<T>::GetData()
	{
		m_GetCounter++;
	 	m_TotalGetCounter++;
		return *m_data_ptr;
	}

	template<class T>
	inline bool AssetMetaData<T>::GetData(T* inPtr)
	{
		m_GetCounter++;
		m_TotalGetCounter++;
		inPtr = *m_data_ptr; 
		
		if (*m_data_ptr == nullptr)
		{
			return false;
		}
		return true;
	}

	//template<class T>
	//inline bool AssetMetaData<T>::GetData(T const* inPtr)
	//{
	//	bool returnValue = false;
	//	if (*m_data_ptr != nullptr)
	//	{
	//		inPtr = *m_data_ptr
	//			returnValue = true;
	//	}

	//	m_GetCounter++;
	//	m_TotalGetCounter++;

	//	return returnValue;
	//}


	template<class T>
	inline void AssetMetaData<T>::SetAllRefData(T& newRefData, const std::string& FileName, const std::string& FilePath, int32_t ContainerSlot)
	{
		m_GetCounter = 0;
		*m_data_ptr = &newRefData;
		m_FileName = FileName;
		m_FilePath = FilePath;
		m_Asset_Container_Slot = ContainerSlot;
	}


	template<class T>
	inline void AssetMetaData<T>::SetMetaDataToNull()
	{
		m_FileName = "";
		m_FilePath = "";
		m_GetCounter = 0;
		*m_data_ptr = nullptr;
		m_Asset_Container_Slot = -1;
	}


	template<class T>
	int32_t AssetMetaData<T>::s_Asset_ID = 0;

	template<class T>
	int32_t AssetMetaData<T>::s_Asset_Counter = 0;

}



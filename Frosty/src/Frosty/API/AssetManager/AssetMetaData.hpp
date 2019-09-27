#ifndef ASSETMETADATA_HPP
#define ASSETMETADATA_HPP

namespace Frosty
{


	struct FileMetaData
	{
		int8_t Type = -1;
		std::string FileName = "";
		std::string FilePath = "";
		std::string PreFab_Name = "";
		std::string FullFilePath = "";
	};

	template<class T>
	class AssetMetaData
	{
		//Variables
	public:



		//Variables
	private:

		static int32_t s_Asset_ID;
		static int32_t s_Asset_Counter;


		std::shared_ptr<T> m_Data_ptr = nullptr;

		int32_t m_Asset_ID;
		//int32_t m_Asset_Container_Slot;

		/*std::string m_FileName;
		std::string m_FilePath;*/

		FileMetaData m_FileMetaData;

		unsigned int m_GetCounter = 0; //For Current Object
		unsigned long int m_TotalGetCounter = 0;

		//Functions
	public:

		AssetMetaData();
		AssetMetaData(const AssetMetaData& other);
		virtual ~AssetMetaData();

		bool operator ==(const AssetMetaData& other) { return (m_FileName == other.m_FileName); }

		//Data Getters
		std::shared_ptr<T> GetData();
		bool GetData(std::shared_ptr<T> inPtr);

		//const std::string& GetFileName() const { return m_FileName; }
		//const std::string& GetFilePath() const { return m_FilePath; }
		//const int32_t& GetAssetContainerSlot() const { return m_Asset_Container_Slot; }

		void SetFileMetaData(const FileMetaData& MetaData) { m_FileMetaData = MetaData; }
		const FileMetaData& GetFileMetaData() { return m_FileMetaData; }

		//Asset Id
		const int32_t& GetAssetId() const { return m_AssetID; }
		void SetNewAssetID() { m_AssetID = s_Asset_ID++; }

		//Counters
		unsigned int& GetTimesVisited() const { return m_GetCounter; }
		const unsigned long int& GetTotalTimesVisited() const { return m_TotalGetCounter; }


		void SetMetaDataToNull();
		//void SetRefData(const int& newRefData) { int m_Dar = FY_NEW newRefData; }

		void SetRefData(const T& newRefData);
		//void SetFileName(const std::string & FileName) { m_FileName = FileName; }
		//void SetFilePath(const std::string & FilePath) { m_FilePath = FilePath; }
		//void SetContainerSlot(const int32_t& NewSlot) { m_Asset_Container_Slot = NewSlot; }

		void SetAllRefData(const T & newRefData, const FileMetaData& MetaData);

		//Functions
	private:


	};

#endif // !ASSETMETADATA_HPP


	template<class T>
	inline AssetMetaData<T>::AssetMetaData()
	{
		m_Data_ptr = nullptr;
		m_Asset_ID = s_Asset_ID++;
		//m_FileName = "";
		//m_FilePath = "";
		//
		s_Asset_Counter++;
		//m_Asset_Container_Slot = -1;
	}


	template<class T>
	inline AssetMetaData<T>::AssetMetaData(const AssetMetaData& other)
	{
		if (this != &other) {
			s_Asset_Counter++;
		}
		m_Asset_ID = s_Asset_ID++;
		//m_Data_ptr = FY_NEW T(*other.m_Data_ptr);
		m_Data_ptr = std::make_shared<T>(*other.m_Data_ptr);
		m_FilePath = other.m_FilePath;
		m_FileName = other.m_FileName;
		m_Asset_Container_Slot = other.m_Asset_Container_Slot;

	}

	template<class T>
	inline AssetMetaData<T>::~AssetMetaData()
	{
		s_Asset_Counter--;
		//if (m_Data_ptr != nullptr)
		//{
		//	delete m_Data_ptr;
		//}
			m_Data_ptr = nullptr;
	}



	template<class T>
	inline std::shared_ptr<T> AssetMetaData<T>::GetData()
	{
		m_GetCounter++;
		m_TotalGetCounter++;
		return m_Data_ptr;
	}


	template<class T>
	inline bool AssetMetaData<T>::GetData(std::shared_ptr<T> inPtr)
	{
		m_GetCounter++;
		m_TotalGetCounter++;
		inPtr = m_Data_ptr;
		return (m_Data_ptr == nullptr);
	}

	template<class T>
	inline void AssetMetaData<T>::SetAllRefData(const T& newRefData, const FileMetaData& MetaData)
	{
		m_GetCounter = 0;
		//m_Data_ptr = FY_NEW T(newRefData);
		m_Data_ptr = = std::make_shared<T>(newRefData);
		m_FileName = FileName;
		m_FilePath = FilePath;
		m_Asset_Container_Slot = ContainerSlot;
	}


	template<class T>
	inline void AssetMetaData<T>::SetMetaDataToNull()
	{
	//	m_FileName = "";
	//	m_FilePath = "";
		m_FileMetaData = FileMetaData();
		m_GetCounter = 0;
		m_Data_ptr = nullptr;
	//	m_Asset_Container_Slot = -1;
	}

	template<class T>
	inline void AssetMetaData<T>::SetRefData(const T& newRefData)
	{
		m_GetCounter = 0;
		//m_Data_ptr = FY_NEW T(newRefData);
		m_Data_ptr = std::make_shared<T>(newRefData);
	}


	template<class T>
	int32_t AssetMetaData<T>::s_Asset_ID = 0;

	template<class T>
	int32_t AssetMetaData<T>::s_Asset_Counter = 0;

}



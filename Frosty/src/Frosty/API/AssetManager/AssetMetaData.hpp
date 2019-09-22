#ifndef ASSETMETADATA_HPP
#define ASSETMETADATA_HPP


template<class T>
class AssetMetaData
{



	//Variables
public:



	//Variables
private:

	static int32_t s_Asset_Counter;
	
	T** const m_data_ptr = new T*;
	int32_t m_AssetID;						// Asset ID set for container or other way around?
	std::string m_FileName;
	unsigned int m_GetCounter = 0; //For Current Object
	unsigned long int m_TotalGetCounter = 0;

	//Functions
public:
	
	AssetMetaData(T& refData , int32_t AssetID, std::string FileName);
	AssetMetaData(const AssetMetaData& other);
	virtual ~AssetMetaData();

	bool operator ==(const AssetMetaData& other) { return (m_FileName == other.m_FileName);};

	T const* GetData();
	bool GetData(T const* inPtr);

	const unsigned int& GetTimesVisited() { return m_GetCounter; };
	const unsigned long int& GetTotalTimesVisited() { return m_TotalGetCounter; };
	const std::string& GetFileName() { return m_FileName; };
	const int32_t GetAssetId() { return m_AssetID; };

	void SetRefData(T& newRefData, int32_t AssetID, std::string FileName);
	void SetMetaDataToNull();

	//Functions
private:


};




#endif // !ASSETMETADATA_HPP


template<class T>
inline AssetMetaData<T>::AssetMetaData(T& refData, int32_t AssetID, std::string FileName)
{
	*m_data_ptr = &refData;
	m_AssetID = AssetID;
	m_FileName = FileName;
	s_Asset_Counter++;
}

template<class T>
inline AssetMetaData<T>::AssetMetaData(const AssetMetaData& other)
{


	m_AssetID = other.m_AssetID;
	*m_data_ptr = *other.m_data_ptr;
	m_FileName = other.m_FilePath;
	s_Asset_Counter++;

}

template<class T>
inline AssetMetaData<T>::~AssetMetaData()
{
	s_Asset_Counter--;
	delete m_data_ptr;
	*m_data_ptr = nullptr;
}

template<class T>
inline T const* AssetMetaData<T>::GetData()
{
	m_GetCounter++;
	m_TotalGetCounter++;
	return *m_data_ptr;
}

template<class T>
inline bool AssetMetaData<T>::GetData(T const* inPtr)
{
	bool returnValue = false;
	if (*m_data_ptr != nullptr)
	{

			m_GetCounter++;
			m_TotalGetCounter++;
			inPtr = *m_data_ptr
			returnValue = true;
		
	}



	return returnValue;
}


template<class T>
inline void AssetMetaData<T>::SetRefData(T& newRefData, int32_t AssetID, std::string FileName)
{
	m_GetCounter = 0;
	*m_data_ptr = &newRefData;
	m_AssetID = AssetID;
	m_FileName = FileName;
}

template<class T>
inline void AssetMetaData<T>::SetMetaDataToNull()
{
	m_GetCounter = 0;
	*m_data_ptr = nullptr;
	m_AssetID = -1;
	m_FileName = "";
}


template<class T>
int32_t AssetMetaData<T>::s_Asset_Counter = 0;




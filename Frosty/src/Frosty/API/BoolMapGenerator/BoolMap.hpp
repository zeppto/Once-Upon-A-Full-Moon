#ifndef BOOLMAP
#define BOOLMAP


class BoolMap
{
private:

	uint16_t m_Width;
	uint16_t m_Height;

	std::shared_ptr<bool> m_BoolMap;

public:
	
	inline BoolMap();
	BoolMap(const uint16_t& Width, const uint16_t& Height, std::shared_ptr<bool> Boolmap);
	BoolMap(const BoolMap& other);
	BoolMap& operator= (const BoolMap& other);

	inline const uint16_t& GetWidth() { return m_Width; }
	inline const uint16_t& GetHeight() { return m_Height; }
	inline const std::shared_ptr<bool>& GetBoolMap() { return m_BoolMap; }


};




#endif // !BOOLMAP

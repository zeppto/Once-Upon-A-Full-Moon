#ifndef BOOLMAP
#define BOOLMAP


class BoolMap
{
private:

	uint16_t m_Width;
	uint16_t m_Height;
	uint8_t m_PixCoordRatio;

	std::shared_ptr<bool[]> m_BoolMap;

public:
	
	inline BoolMap() = delete;
	BoolMap(const uint16_t& Width, const uint16_t& Height, const uint8_t PixRatio,std::shared_ptr<bool[]>& Boolmap);
	BoolMap(const BoolMap& other);
	BoolMap& operator= (const BoolMap& other);


	const bool& CheckCollition(const glm::vec3& LocalPos) const;

	inline const uint16_t& GetWidth() { return m_Width; }
	inline const uint16_t& GetHeight() { return m_Height; }
	inline const uint16_t& GetPixCoordRatio() { return m_PixCoordRatio; }
	inline const std::shared_ptr<bool[]>& GetBoolMap() { return m_BoolMap; }


};




#endif // !BOOLMAP

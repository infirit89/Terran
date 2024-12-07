#pragma once

#include <string>
#include <array>
#include <functional>

namespace TerranEngine 
{
	class UUID
	{
	public:
		UUID();
		UUID(const std::array<uint8_t, 16>& data);
		UUID(const UUID& other) = default;
		static UUID Invalid() { return s_Empty; }

		std::array<uint8_t, 16> GetData() const { return m_Data; }
		std::array<uint8_t, 16> GetData() { return m_Data; }

		void SetData(const std::array<uint8_t, 16>& idArray) { m_Data = idArray; }

		const uint8_t* GetRaw() const { return m_Data.data(); }
		static UUID CreateFromRaw(const uint8_t* data);
		
		bool operator==(const UUID& other) const	{ return m_Data == other.m_Data; }
		bool operator==(const UUID& other)			{ return m_Data == other.m_Data; }

		bool operator!=(const UUID& other) const	{ return m_Data != other.m_Data; }
		bool operator!=(const UUID& other)			{ return m_Data != other.m_Data; }

		bool operator<(const UUID& other) const	{ return m_Data < other.m_Data; }
		bool operator<(const UUID& other)		{ return m_Data < other.m_Data; }

		bool operator>(const UUID& other) const	{ return m_Data > other.m_Data; }
		bool operator>(const UUID& other)		{ return m_Data > other.m_Data; }
		
		operator bool() const { return IsValid(); }

		bool IsValid() const
		{
			std::array<uint8_t, 16> empty{ {0} };

			return m_Data != empty;
		}


		static UUID FromString(const std::string& str);

	private:
		void Generate(); 
		/*
		* empty UUID:
		* 1	 2  3  4    5  6    7  8    9  10   11 12 13 14 15 16
		* 00 00 00 00 - 00 00 - 00 00 - 00 00 - 00 00 00 00 00 00
		* 
		* 1-4: time-low
		* 5-6: time-mid
		* 
		* 7-8: time-high and version
		* 
		* 9: clock-seq and reserved
		* 
		* 10: clock-seq low
		* 
		* 11-16: node
		*/
		std::array<uint8_t, 16> m_Data;
		static UUID s_Empty;
	};
}

#include <sstream>
#include <bitset>
#include <iostream>

namespace std 
{
	template<>
	struct hash<TerranEngine::UUID> 
	{
		size_t operator()(const TerranEngine::UUID& uuid) const noexcept
		{
			std::hash<uint8_t> hash;

			size_t result = 0;

			std::array<uint8_t, 16> idArr = uuid.GetData();

			for (size_t i = 0; i < idArr.size(); i++)
				result = (result << 1) ^ hash(idArr[i]);

			return result;
		}
	};

	template<class CharT = char,
		class Traits = std::char_traits<CharT>,
		class Allocator = std::allocator<CharT>>
		std::basic_string<CharT, Traits, Allocator> to_string(TerranEngine::UUID const& id)
	{
		std::array<uint8_t, 16> idArr = id.GetData();
		
		std::basic_stringstream<CharT, Traits, Allocator> sstr;

		int ind = 0;
		for (int i = 0; i < 20; i++)
		{
			if (i == 4 || i == 7 || i == 10 || i == 13) 
			{
				sstr << '-';
				continue;
			}

			sstr << std::hex
				<< (int)(idArr[ind] >> 4 & 0x0f)
				<< (int)(idArr[ind] & 0x0f);
			
			ind++;
		}


		return sstr.str();
	};
}

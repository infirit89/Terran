#pragma once

#include "Log.h"

#include <string>
#include <array>
#include <xhash>

namespace TerranEngine 
{
	class UUID 
	{
	public:
		UUID(bool generate = true);
		UUID(const std::array<uint8_t, 16>& data);
		UUID(const UUID& other) = default;
		static UUID Empty() { return UUID(false); }

		const inline std::array<uint8_t, 16> GetData() const { return m_Data; }
		inline std::array<uint8_t, 16> GetData() { return m_Data; }

		const inline bool operator==(const UUID& other) const	{ return m_Data == other.m_Data; }
		inline bool operator==(const UUID& other)				{ return m_Data == other.m_Data; }

		const inline bool operator!=(const UUID& other) const	{ return !(m_Data == other.m_Data); }
		inline bool operator!=(const UUID& other)				{ return !(m_Data == other.m_Data); }

		const inline bool operator<(const UUID& other) const	{ return m_Data < other.m_Data; }
		inline bool operator<(const UUID& other)				{ return m_Data < other.m_Data; }

		const inline bool operator>(const UUID& other) const	{ return m_Data > other.m_Data; }
		inline bool operator>(const UUID& other)				{ return m_Data > other.m_Data; }
		
		bool Valid() 
		{
			std::array<uint8_t, 16> empty{ {0} };

			return !(m_Data == empty);
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

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const UUID& uuid);

	};
}

#include <sstream>
namespace std 
{
	template<>
	struct hash<TerranEngine::UUID> 
	{
		size_t operator()(const TerranEngine::UUID& uuid) const 
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
		for (int i = 0; i < 19; i++)
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

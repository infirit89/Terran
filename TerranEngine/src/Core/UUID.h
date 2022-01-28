#pragma once

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

		static UUID FromString(const std::string& str);

	private:
		void Generate();
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
		sstr << std::hex
			<< (int)idArr[0]
			<< (int)idArr[1]
			<< (int)idArr[2]
			<< (int)idArr[3]
			<< '-'
			<< (int)idArr[4]
			<< (int)idArr[5]
			<< '-'
			<< (int)idArr[6]
			<< (int)idArr[7]
			<< '-'
			<< (int)idArr[8]
			<< (int)idArr[9]
			<< '-'
			<< (int)idArr[10]
			<< (int)idArr[11]
			<< (int)idArr[12]
			<< (int)idArr[13]
			<< (int)idArr[14]
			<< (int)idArr[15];

		return sstr.str();
	};
}

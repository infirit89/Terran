#pragma once

#include <string>
#include <array>

namespace TerranEngine 
{
	class UUID 
	{
	public:
		UUID();
		UUID(const std::array<uint8_t, 16>& data);

		std::array<uint8_t, 16> GetData() const { return m_Data; }

		bool operator==(const UUID& other) const;
		bool operator!=(const UUID& other) const;
		bool operator<(const UUID& other) const;
		bool operator>(const UUID& other) const;

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

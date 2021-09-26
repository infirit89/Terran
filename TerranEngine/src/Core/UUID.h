#pragma once

#include <array>

namespace TerranEngine 
{
	class UUID 
	{
	public:
		UUID();
		UUID(const std::array<uint8_t, 16>& data);

		std::array<uint8_t, 16> GetData() const { return m_Data; }

	private:
		void Generate();
		std::array<uint8_t, 16> m_Data;

		template<typename OStream>
		friend OStream& operator<<(OStream& os, const UUID& uuid);
	};
}
#include "trpch.h"
#include "Core/UUID.h"

namespace TerranEngine
{
	UUID UUID::s_Empty({ 0 });

	UUID::UUID()
		: m_Data({ 0 })
	{
		Generate();
	}

	UUID::UUID(const std::array<uint8_t, 16>& data)
		: m_Data(data)
	{
	}

	UUID UUID::CreateFromRaw(uint8_t* data)
	{
		std::array<uint8_t, 16> idData = { 0 };
		constexpr uint32_t uuidArrayLength = 16 * sizeof(uint8_t);

		memcpy(&idData, data, uuidArrayLength);

		return UUID({ idData });
	}

	static unsigned char CharToHex(const char& ch)
	{
		if (ch >= '0' && ch <= '9')
			return ch - '0';
		if (ch >= 'a' && ch <= 'f')
			return 10 + ch - 'a';
		if (ch >= 'A' && ch <= 'F')
			return 10 + ch - 'A';

		return 0;
	}

	UUID UUID::FromString(const std::string& str)
	{
		size_t index = 0;

		std::array<uint8_t, 16> data{ {0} };

		if (str.empty())
			return UUID({ 0 });

		for (size_t i = 0; i < str.size(); ++i)
		{
			if (str[i] == '-') continue;

			if (index >= 16 || !isxdigit(str[i]))
			{
				TR_CORE_ERROR(TR_LOG_CORE, "UUID not in hexidecimal format!");
				return UUID({ 0 });
			}

			data[index] = (uint8_t)(CharToHex(str[i]) << 4);
			data[index] |= (uint8_t)(CharToHex(str[++i]));

			index++;
		}

		if (index < 16)
			return UUID({ 0 });

		return UUID{ data };
	}
}

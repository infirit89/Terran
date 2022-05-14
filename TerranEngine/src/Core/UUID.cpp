#include "trpch.h"
#include "Core/UUID.h"

namespace TerranEngine
{
	UUID::UUID()
		: m_Data({ 0 })
	{
		Generate();
	}

	UUID::UUID(const std::array<uint8_t, 16>& data)
		: m_Data(data)
	{
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
		{
			TR_ERROR("Empty UUID!");
			return UUID({ 0 });
		}

		for (size_t i = 0; i < str.size(); ++i)
		{
			if (str[i] == '-') continue;

			if (index >= 16 || !isxdigit(str[i]))
			{
				TR_ERROR("UUID not in hexidecimal format!");
				return UUID({ 0 });
			}

			data[index] = (uint8_t)(CharToHex(str[i]) << 4);
			data[index] |= (uint8_t)(CharToHex(str[++i]));

			index++;
		}

		if (index < 16)
		{
			TR_ERROR("Invalid UUID!");
			return UUID({ 0 });
		}

		return UUID{ data };
	}
}

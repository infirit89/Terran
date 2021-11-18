#include "trpch.h"
#include "Core/UUID.h"

namespace TerranEngine
{
    UUID::UUID(bool generate)
        : m_Data({ 0 })
    {
        if(generate)
            Generate();
    }

    UUID::UUID(const std::array<uint8_t, 16>& data)
        : m_Data(data)
    {
    }

    static unsigned char HexToChar(const char& ch) 
    {
        if (ch >= static_cast<char>('0') && ch <= static_cast<char>('9'))
            return ch - static_cast<char>('0');
        if (ch >= static_cast<char>('a') && ch <= static_cast<char>('f'))
            return 10 + ch - static_cast<char>('a');
        if (ch >= static_cast<char>('A') && ch <= static_cast<char>('F'))
            return 10 + ch - static_cast<char>('A');

        return 0;
    }

    static bool IsHex(const char& ch) 
    {
        return
            (ch >= static_cast<char>('0') && ch <= static_cast<char>('9')) ||
            (ch >= static_cast<char>('a') && ch <= static_cast<char>('f')) || 
            (ch >= static_cast<char>('A') && ch <= static_cast<char>('F'));
    }

    UUID& UUID::FromString(const std::string& str)
    {
        size_t index = 0;
        bool firstDigit = true;

        std::array<uint8_t, 16> data{ {0} };

        if (str.empty()) return UUID();

        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str[i] == '-') continue;

            if (index >= 16 || !IsHex(str[i])) return UUID();

            if (firstDigit)
            {
                data[index] = HexToChar(str[i]) << 4;
                firstDigit = false;
            }
            else
            {
                data[index++] |= HexToChar(str[i]);
                firstDigit = true;
            }
        }

        if (index < 14) 
        {
            TR_ERROR("Invalid UUID!");
            return UUID();
        }

        return UUID{ data };
    }
}

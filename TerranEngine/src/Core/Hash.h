#pragma once

#include <string>
#include <cstdint>

namespace TerranEngine 
{
	class Hash
	{
	public:
		static uint32_t FNVHash(const std::string& str);
		static uint32_t FNVHash(std::string_view str);
	};
}

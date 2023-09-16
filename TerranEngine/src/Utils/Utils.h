#pragma once
#include <string>

namespace TerranEngine 
{
	namespace Utils 
	{
		std::string ToLower(const std::string& str);
		std::string ToUpper(const std::string& str);

		void TrimEnd(std::string& str);
	}
}
#include "trpch.h"
#include "Utils.h"

namespace TerranEngine 
{
    namespace Utils 
    {
        std::string ToLower(const std::string& str)
        {
            std::string temp; temp.resize(str.size());
            std::transform(str.begin(), str.end(), temp.begin(), 
                [](char c) { return tolower(c); });

            return temp;
        }

        std::string ToUpper(const std::string& str)
        {
            std::string temp; temp.resize(str.size());
            std::transform(str.begin(), str.end(), temp.begin(),
                [](char c) { return toupper(c); });

            return temp;
        }
    }
}


#include "trpch.h"
#include "Utils.h"

namespace TerranEngine 
{
    namespace Utils 
    {
        std::string ToLower(const std::string& str)
        {
            std::string temp; temp.resize(str.size());
            std::ranges::transform(str, temp.begin(), 
                                   [](char c) { return tolower(c); });

            return temp;
        }

        std::string ToUpper(const std::string& str)
        {
            std::string temp; temp.resize(str.size());
            std::ranges::transform(str, temp.begin(),
                                   [](char c) { return toupper(c); });

            return temp;
        }

        void TrimEnd(std::string& str)
        {
            str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(), str.end());
        }
    }
}


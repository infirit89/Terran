#pragma once

#include <string>

namespace TerranEngine 
{
	struct FileData final
	{
		char* Data = nullptr;
		int Length = 0, Index = 0;
		char PreviousChar;
	};

	namespace File 
	{
		FileData* OpenFile(const char* filePath);
		void CloseFile(FileData* data);

		FileData* WriteToFile(const char* filePath, const char* text);
		FileData* AppendToFile(const char* filePath, char* text);

		bool GetCharacter(char& c, FileData* data);
		bool GetLine(FileData* data, std::string& str);

		void CreateFile(const char* filePath);
	}
}
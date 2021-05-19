#pragma once

namespace TerranEngine 
{
	struct FileData 
	{
		char* Data;
		int Length;
	};

	namespace File 
	{
		FileData* OpenFile(const char* filePath);
		void CloseFile(FileData* data);

		FileData* WriteToFile(const char* filePath, char* text);
		FileData* AppendToFile(const char* filePath, char* text);

		void CreateFile(const char* filePath);
	}
}
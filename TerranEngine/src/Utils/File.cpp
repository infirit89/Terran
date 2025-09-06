#include "trpch.h"
#include <cstdio>
#include "File.h"

namespace TerranEngine 
{
	FileData* File::OpenFile(const char* filePath)
	{
		FileData* data = new FileData;

		FILE* filePtr = fopen(filePath, "rb");

		data->Index = 0;

		if (filePtr) 
		{
			fseek(filePtr, 0, SEEK_END);

			data->Length = ftell(filePtr);

			if (data->Length != -1)
			{
				rewind(filePtr);
				data->Data = new char[data->Length + 1];

				size_t result = fread(data->Data, 1, data->Length, filePtr);

				if (result != data->Length) 
				{
					TR_CORE_ERROR(TR_LOG_CORE, "Reading error");

					fclose(filePtr);
					delete[] data->Data;
					data->Data = nullptr;
				}

				data->Data[data->Length] = '\0';
				fclose(filePtr);
			}
			else
				TR_CORE_ERROR(TR_LOG_CORE, "Couldn't read from file {0}", filePath);
		}
		else
			TR_CORE_ERROR(TR_LOG_CORE, "Couldn't open file {0}", filePath);

		return data;
	}

	void File::CloseFile(FileData* data)
	{
		if (data) 
		{
			if (data->Data) 
			{
				delete[] data->Data;
				data->Data = nullptr;
			}

			delete data;
			data = nullptr;
		}
	}

	FileData* File::WriteToFile(const char* filePath, const char* text)
	{
		FileData* data = new FileData;

		FILE* filePtr = fopen(filePath, "w+b");

		data->Index = 0;


		if (filePtr)
		{
			 size_t result = fwrite(text, sizeof(char), sizeof(text), filePtr);     // TODO: check if it has been written to the file correctly
			 fclose(filePtr);
		}
		else
			TR_CORE_ERROR(TR_LOG_CORE, "Couldn't open file {0}", filePath);

		return data;
	}

	FileData* File::AppendToFile(const char* filePath, char* text)
	{
		FileData* data = new FileData;
		
		data->Index = 0;

		FILE* filePtr = fopen(filePath, "a+b");

		if (filePtr) 
		{
			while (fgets(text, sizeof(text), filePtr))
				fprintf(filePtr, "%s", text);

			fclose(filePtr);
		}
		else 
			TR_CORE_ERROR(TR_LOG_CORE, "Couldn't open file {0}", filePath);

		return data;
	}

	bool File::GetCharacter(char& c, FileData* data)
	{
		if (data->PreviousChar == '\0')
			return false;

		c = data->Data[data->Index];
		data->PreviousChar = c;

		data->Index++;

		return true;
	}

	bool File::GetLine(FileData* data, std::string& str)
	{
		char c;
		str.clear();
		
		while (File::GetCharacter(c, data) && c != '\r')
			str.push_back(c);

		str.push_back('\r');
		str.push_back('\n');

		return File::GetCharacter(c, data);
	}

	void File::CreateFile(const char* filePath)
	{
		FILE* filePtr = fopen(filePath, "w");

		if (filePtr == nullptr)
			TR_CORE_ERROR(TR_LOG_CORE, "Couldn't create file {0}", filePath);

		fclose(filePtr);
	}
}

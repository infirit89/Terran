#include "trpch.h"
#include "File.h"

namespace TerranEngine 
{
	FileData* File::OpenFile(const char* filePath)
	{
		FileData* data = (FileData*)malloc(sizeof(FileData));

		FILE* filePtr;

		fopen_s(&filePtr, filePath, "rb");

		data->Index = 0;

		if (filePtr) 
		{
			fseek(filePtr, 0, SEEK_END);

			data->Length = ftell(filePtr);

			if (data->Length != -1)
			{
				rewind(filePtr);
				data->Data = (char*)malloc(sizeof(char) * (data->Length + 1));

				size_t result = fread(data->Data, 1, data->Length, filePtr);

				if (result != data->Length) 
				{
					TR_ERROR("Reading error");

					fclose(filePtr);
					free(data->Data);
					data->Data = nullptr;
				}

				data->Data[data->Length] = '\0';
				fclose(filePtr);
			}
			else
				TR_ERROR("Couldn't read from file {0}", filePath);
		}
		else
			TR_ERROR("Couldn't open file {0}", filePath);

		return data;
	}

	void File::CloseFile(FileData* data)
	{
		if (data) 
		{
			if (data->Data) 
			{
				free(data->Data);
				data->Data = nullptr;
			}

			free(data);
			data = nullptr;
		}
	}

	FileData* File::WriteToFile(const char* filePath, char* text)
	{
		FileData* data = (FileData*)malloc(sizeof(FileData));

		FILE* filePtr;

		data->Index = 0;

		fopen_s(&filePtr, filePath, "w+b");

		if (filePtr) 
		{
			 size_t result = fwrite(text, sizeof(char), sizeof(text), filePtr);     // TODO: check if it has been written to the file correctly
			 fclose(filePtr);
		}
		else
			TR_ERROR("Couldn't open file {0}", filePath);

		return data;
	}

	FileData* File::AppendToFile(const char* filePath, char* text)
	{
		FileData* data = (FileData*)malloc(sizeof(FileData));
		
		data->Index = 0;

		FILE* filePtr;

		fopen_s(&filePtr, filePath, "a+b");

		if (filePtr) 
		{
			while (fgets(text, sizeof(text), filePtr))
				fprintf(filePtr, text);

			fclose(filePtr);
		}
		else 
			TR_ERROR("Couldn't open file {0}", filePath);

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
		FILE* filePtr;

		fopen_s(&filePtr, filePath, "w");

		if (filePtr == nullptr)
			TR_ERROR("Couldn't create file {0}", filePath);

		fclose(filePtr);
	}
}

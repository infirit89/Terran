#include "trpch.h"
#include "Core/FileUtils.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "Core/Application.h"

#ifdef TR_WINDOWS_64
#include <commdlg.h>

namespace TerranEngine 
{
	std::filesystem::path FileUtils::OpenFile(const char* filter) 
	{
		OPENFILENAMEA ofn;

		char fileName[260] = { 0 };
		char currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get()->GetWindow().GetNativeWindow());

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = 260;

		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;


		if (GetOpenFileNameA(&ofn))
			return ofn.lpstrFile;

		return "";
	}

	std::filesystem::path FileUtils::SaveFile(const char* filter) 
	{
		OPENFILENAMEA ofn;

		char fileName[260] = { 0 };
		char currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get()->GetWindow().GetNativeWindow());

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = sizeof(fileName);

		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn))
			return ofn.lpstrFile;

		return "";
	}
}
#endif

#include "trpch.h"
#include "Core/FileUtils.h"

#include "Core/Application.h"

#include "Project/Project.h"

#ifdef TR_WINDOWS_64
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include <commdlg.h>
#include <shellapi.h>
#include <Windows.h>

namespace TerranEngine 
{
	std::filesystem::path FileSystem::OpenFileDialog(const char* filter) 
	{
		OPENFILENAMEA ofn;

		char fileName[260] = { 0 };
		char currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow()));

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = 260;

		/*if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;*/

		std::string assetPathStr = std::filesystem::absolute(Project::GetAssetPath()).string();
		ofn.lpstrInitialDir = assetPathStr.c_str();

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn))
			return ofn.lpstrFile;

		return "";
	}

	std::filesystem::path FileSystem::SaveFileDialog(const char* filter) 
	{
		OPENFILENAMEA ofn;

		char fileName[260] = { 0 };
		char currentDir[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window(static_cast<GLFWwindow*>(Application::Get()->GetWindow().GetNativeWindow()));

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = sizeof(fileName);

		/*if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;*/

		std::string assetPathStr = std::filesystem::absolute(Project::GetAssetPath()).string();
		ofn.lpstrInitialDir = assetPathStr.c_str();

		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn))
			return ofn.lpstrFile;

		return "";
	}

	void FileSystem::RevealInExplorer(const std::filesystem::path& path)
	{
		std::string stringPath = path.string();
		SHELLEXECUTEINFOA shellInfo;
		shellInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
		shellInfo.fMask = NULL;
		shellInfo.hwnd = nullptr;
		shellInfo.lpVerb = "explore";
		shellInfo.lpDirectory = stringPath.c_str();
		shellInfo.lpFile = nullptr;
		shellInfo.lpParameters = nullptr;
		shellInfo.nShow = SW_NORMAL;
		shellInfo.hInstApp = nullptr;

		ShellExecuteExA(&shellInfo);
	}

	static HANDLE s_FileWatchThread;
	static bool s_Watching;

	void FileSystem::StartWatch()
	{
		s_Watching = true;
		s_FileWatchThread = CreateThread(nullptr, 0, Watch, 0, 0, nullptr);
		TR_ASSERT(s_FileWatchThread != NULL, "Couldn't create the watcher thread");
	}

	void FileSystem::StopWatch()
	{
		// terminate the watch thread
		if (!s_Watching) return;

		s_Watching = false;

		TerminateThread(s_FileWatchThread, 1);
		CloseHandle(s_FileWatchThread);
	}

	// this is the win32api thread proc signature
	unsigned long FileSystem::Watch(void* params)
	{
		// read directory changes; append them to a list of file change events
		std::wstring dirStr = s_PathToWatch.wstring();
		HANDLE hDir = CreateFile(dirStr.c_str(),
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			nullptr);

		TR_ASSERT(hDir != INVALID_HANDLE_VALUE, "Couldn't open directory");

		OVERLAPPED overlap;

		constexpr size_t maxFileInformationBufferSize = 20;

		overlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		FILE_NOTIFY_INFORMATION buf[maxFileInformationBufferSize];
		DWORD bytesReturned;

		std::vector<FileSystemChangeEvent> fileChanges;

		bool result = true;

		while(s_Watching && result) 
		{
			result = ReadDirectoryChangesW(hDir,
				&buf,
				sizeof(buf),
				TRUE,
				FILE_NOTIFY_CHANGE_SIZE |
				FILE_NOTIFY_CHANGE_LAST_WRITE |
				FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_FILE_NAME,
				&bytesReturned,
				&overlap,
				nullptr);

			WaitForSingleObject(overlap.hEvent, INFINITE);

			int offset = 0;
			FILE_NOTIFY_INFORMATION* pNotify;

			std::filesystem::path oldFileName = "";
			do 
			{
				FileSystemChangeEvent e;

				pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char*>(buf) + offset);

				size_t fileNameLength = pNotify->FileNameLength / sizeof(wchar_t);
				std::filesystem::path fileName = std::filesystem::path(std::wstring(pNotify->FileName, fileNameLength));
				e.FileName = fileName;

				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					e.Action = FileAction::Added;
					break;
				case FILE_ACTION_MODIFIED:
					e.Action = FileAction::Modified;
					break;
				case FILE_ACTION_REMOVED:
					e.Action = FileAction::Removed;
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					oldFileName = fileName;
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					e.Action = FileAction::Renamed;
					e.OldFileName = oldFileName;
					break;
				default: ;
				}

				offset += pNotify->NextEntryOffset;

				if(pNotify->Action != FILE_ACTION_RENAMED_OLD_NAME)
					fileChanges.emplace_back(e);
			} while (pNotify->NextEntryOffset != 0);

			s_ChangeCallback(fileChanges);
			fileChanges.clear();
		}

		return 1;
	}
}
#endif

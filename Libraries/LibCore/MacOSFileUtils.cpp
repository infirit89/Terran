#include "Assert.h"
#include "FileUtils.h"

#include <filesystem>

namespace Terran {
namespace Core {

// TODO: implement the methods, currently left blank due to having linking problems
// add an ifdef at the top of the file so its only included in macos builds
std::filesystem::path FileSystem::OpenFileDialog(char const* filter)
{
    TR_ASSERT(false, "Not implemented");
    return "";
}

std::filesystem::path FileSystem::SaveFileDialog(char const* filter)
{
    TR_ASSERT(false, "Not implemented");
    return "";
}

void FileSystem::RevealInExplorer(std::filesystem::path const& path)
{
    TR_ASSERT(false, "Not implemented");
}

void FileSystem::StartWatch()
{
}

void FileSystem::StopWatch()
{
}

// this is the win32api thread proc signature
unsigned long FileSystem::Watch(void* params)
{
    return 1;
}

}

}

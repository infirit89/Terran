#include "Application.h"
#include "LibCore/Log.h"

#include <cstddef>
#include <string_view>
#include <vector>

int main(int argc, char** argv)
{
    Terran::Core::Log::Init();

    std::vector<std::string_view> arguments;
    arguments.reserve(argc);
    for (int i = 0; i < argc; i++) {
        arguments.emplace_back(argv[i]);
    }
    auto* application = create_application(arguments);
    if (!application)
        return 1;

    auto result = application->run();
    delete application;

    Terran::Core::Log::Shutdown();

    if(!result.is_ok())
        return 1;

    return 0;
}

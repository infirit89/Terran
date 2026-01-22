project "Test.LibCore"
language "C++"
cppdialect "C++20"
kind "ConsoleApp"
staticruntime "Off"

files {
    "**.cpp",
    "**.hpp",
}

externalincludedirs {
    "%{wks.location}/Libraries",
    "%{Dependencies.spdlog.include}",
    "../../vendor/gtest/googletest/include/",
}

links {
    "LibCore",
    "yaml-cpp",
    "googletest",
}

filter { "configurations:Debug" }
runtime "Debug"
symbols "On"
defines { "TERRAN_TESTING_DEBUG" }
defines "TR_DEBUG"
filter {}

filter { "configurations:Release" }
runtime "Release"
defines { "TERRAN_TESTING_RELEASE" }

filter { "system:macosx" }
architecture "ARM64"

libdirs { "/usr/local/lib" }
links {
    "CoreFoundation.framework", -- no path needed for system frameworks
}

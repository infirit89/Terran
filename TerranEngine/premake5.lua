project "TerranEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

    files 
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/stb/**.h",
        "vendor/stb/**.cpp"
    }

    includedirs
    {
        "src",
        "vendor/spdlog/include/",
        "vendor/GLFW/include/",
        "vendor/ImGui/",
        "vendor/GLAD/include/",
        "vendor/stb/"
    } 

    links 
    {
        "GLFW",
        "ImGui",
        "GLAD",
        "opengl32.lib"
    }

    pchheader "trpch.h"
    pchsource "src/trpch.cpp"

    defines 
    {
        "GLFW_INCLUDE_NONE"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "TR_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "TR_RELEASE"
        runtime "Release"
        optimize "on"
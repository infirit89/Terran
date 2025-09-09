project "GLAD"
    language "C"
    kind "StaticLib"
    staticruntime "off"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

    files
    {
        "src/glad.c",
        "include/glad/glad.h",
        "include/KHR/khrplatform.h"
    }

    includedirs
    {
        "include"
    }

    filter "system:windows"
        architecture "x86_64"
        systemversion "latest"

    filter "system:macosx"
		architecture "ARM64"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"
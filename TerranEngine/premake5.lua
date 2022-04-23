mono_path = os.getenv("MONO_PATH")
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
        "vendor/stb/**.cpp",
        "vendor/glm/**.hpp",
        "vendor/glm/**.inl",
        "vendor/freetype-gl/**.h"
    }

    includedirs
    {
        "src",
        "%{wks.location}/TerranEngine/vendor/spdlog/include/",
        "%{wks.location}/TerranEngine/vendor/GLFW/include/",
        "%{wks.location}/TerranEngine/vendor/ImGui/",
        "%{wks.location}/TerranEngine/vendor/GLAD/include/",
        "%{wks.location}/TerranEngine/vendor/stb/",
        "%{wks.location}/TerranEngine/vendor/glm/",
        "%{wks.location}/TerranEngine/vendor/entt/include/",
        "%{wks.location}/TerranEngine/vendor/freetype/include/",
        "%{wks.location}/TerranEngine/vendor/freetype-gl/src/",
        "%{wks.location}/TerranEngine/vendor/nlohman-json/include/",
        "%{mono_path}/include/mono-2.0/",
        "%{wks.location}/TerranEngine/vendor/Box2D/include/"
    } 

    libdirs 
    {
        "%{mono_path}/lib/"
    }

    links 
    {
        "%{mono_path}/lib/mono-2.0-sgen.lib",
        "GLFW",
        "ImGui",
        "GLAD",
        "opengl32.lib",
        "Freetype",
        "Freetype-GL",
        "Box2D"
    }

    pchheader "trpch.h"
    pchsource "src/trpch.cpp"

    disablewarnings 
    {
        "26451",
        "26498",
        "26495",
        "26812",
        "4267"
    }

    defines 
    {
        "_CRT_SECURE_NO_WARNINGS",
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
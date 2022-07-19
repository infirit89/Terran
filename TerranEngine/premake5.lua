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
        "vendor/glm/**.inl"
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
        "%{wks.location}/TerranEngine/vendor/msdf-atlas-gen/msdfgen/",
        "%{wks.location}/TerranEngine/vendor/msdf-atlas-gen/",
        "%{wks.location}/TerranEngine/vendor/nlohman-json/include/",
        "%{wks.location}/TerranEngine/vendor/mono/include/",
        "%{wks.location}/TerranEngine/vendor/Box2D/include/"
    } 

    libdirs 
    {
        "%{wks.location}/TerranEngine/vendor/mono/lib/"
    }

    links 
    {
        "%{wks.location}/TerranEngine/vendor/mono/bin/mono-2.0-sgen.lib",
        "GLFW",
        "ImGui",
        "GLAD",
        "opengl32.lib",
        "msdf-atlas-gen",
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

        links
        {
            "Imm32.lib",
            "Rpcrt4.lib"
        }

    filter "configurations:Debug"
        defines "TR_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "TR_RELEASE"
        runtime "Release"
        optimize "on"
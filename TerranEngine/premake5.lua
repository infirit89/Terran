project "TerranEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "Off"

    architecture "x86_64"

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

    dependson 
    {
        "TerranScriptCore"
    }

    includedirs
    {
        "src",
        "%{IncludeDirectories.spdlog}",
        "%{IncludeDirectories.glfw}",
        "%{IncludeDirectories.imgui}",
        "%{IncludeDirectories.glad}",
        "%{IncludeDirectories.stb}",
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.entt}",
        "%{IncludeDirectories.msdfgen}",
        "%{IncludeDirectories.msdf_atlas_gen}",
        "%{IncludeDirectories.box2d}",
        "%{IncludeDirectories.optick}",
        "%{IncludeDirectories.yaml}",
        "%{IncludeDirectories.shaderc}",
        "%{IncludeDirectories.coral}",
    }

    links
    {
        "GLFW",
        "ImGui",
        "GLAD",
        "opengl32.lib",
        "msdf-atlas-gen",
        "Box2D",
        "OptickCore",
        "yaml-cpp",
        "Coral.Native",
        "%{Libraries.shaderc}"
    }

    pchheader "trpch.h"
    pchsource "src/trpch.cpp"

    linkoptions
    {
        "-IGNORE:4006"
    }

    defines 
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE",
        "YAML_CPP_STATIC_DEFINE"
    }

    filter "system:windows"
        systemversion "latest"

        links
        {
            "%{Libraries.imm32}",
            "%{Libraries.rpcrt4}"
        }

    filter "configurations:Debug"
        defines "TR_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "TR_RELEASE"
        runtime "Release"
        optimize "on"
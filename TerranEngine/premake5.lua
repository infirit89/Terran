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
        "%{IncludeDirectories.spdlog}",
        "%{IncludeDirectories.glfw}",
        "%{IncludeDirectories.imgui}",
        "%{IncludeDirectories.glad}",
        "%{IncludeDirectories.stb}",
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.entt}",
        "%{IncludeDirectories.msdfgen}",
        "%{IncludeDirectories.msdf_atlas_gen}",
        "%{IncludeDirectories.mono}",
        "%{IncludeDirectories.box2d}",
        "%{IncludeDirectories.optick}",
        "%{IncludeDirectories.yaml}",
    }

    -- libdirs 
    -- {
    --     "%{wks.location}/TerranEngine/vendor/mono/lib/"
    -- }

    links 
    {
        "%{Libraries.mono_static}",
        "GLFW",
        "ImGui",
        "GLAD",
        "opengl32.lib",
        "msdf-atlas-gen",
        "Box2D",
        "OptickCore",
        "yaml-cpp"
    }

    pchheader "trpch.h"
    pchsource "src/trpch.cpp"

    disablewarnings 
    {
        "26451",
        "26498",
        "26495",
        "26812",
        "4267",
        "4006"
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
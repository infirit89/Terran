project "TerranEngine"
pic "On"
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir("%{prj.location}/bin/" .. outputdir)
objdir("%{prj.location}/bin-int/" .. outputdir)

debuggertype "NativeWithManagedCore"

files {
    "src/**.h",
    "src/**.cpp",

    "vendor/stb/**.h",
    "vendor/stb/**.cpp",

    "vendor/glm/**.hpp",
    "vendor/glm/**.inl",
}

removefiles {
    "src/Asset/**.h",
    "src/Asset/**.cpp",

    "src/Graphics/**.h",
    "src/Graphics/**.cpp",

    "src/Physics/**.h",
    "src/Physics/**.cpp",

    "src/Project/**.h",
    "src/Project/**.cpp",

    "src/Scripting/**.h",
    "src/Scripting/**.cpp",

    "src/Scene/**.h",
    "src/Scene/**.cpp",

    "src/Platform/**.h",
    "src/Platform/**.cpp",

    "src/ImGui/**.h",
    "src/ImGui/**.cpp",

    "src/Core/Window.h",
    "src/Core/Window.cpp",

    "src/Utils/**.h",
    "src/Utils/**.cpp",
}

dependson {
    "TerranScriptCore",
}

includedirs {
    "src",
    "%{wks.location}/Libraries",
    "%{Dependencies.spdlog.include}",
    "%{Dependencies.yaml.include}",
    "%{Dependencies.glm.include}",
    "%{IncludeDirectories.glfw}",
    "%{IncludeDirectories.imgui}",
    "%{IncludeDirectories.glad}",
    "%{IncludeDirectories.stb}",
    "%{IncludeDirectories.entt}",
    "%{IncludeDirectories.msdfgen}",
    "%{IncludeDirectories.msdf_atlas_gen}",
    "%{IncludeDirectories.box2d}",
    -- "%{IncludeDirectories.shaderc}",
    "%{IncludeDirectories.coral}",
}

pchheader "trpch.h"
pchsource "src/trpch.cpp"

linkoptions {
    "-IGNORE:4006",
}

defines {
    "_CRT_SECURE_NO_WARNINGS",
    "GLFW_INCLUDE_NONE",
    "YAML_CPP_STATIC_DEFINE",
    "MSDF_ATLAS_NO_ARTERY_FONT",
    "MSDFGEN_PUBLIC=",
}

filter "system:macosx"
architecture "ARM64"

libdirs { "/usr/local/lib" }
links {
    "CoreFoundation.framework", -- no path needed for system frameworks
    "OpenGL.framework",
    "Cocoa.framework",
    "IOKit.framework",
    "QuartzCore.framework",
}

filter "system:windows"
architecture "x86_64"
systemversion "latest"

includedirs {
    "%{IncludeDirectories.optick}",
}

links {
    "OptickCore",
    "opengl32.lib",
    "%{ExternalLibraries.imm32}",
    "%{ExternalLibraries.rpcrt4}",
    "%{ExternalLibraries.shaderc}",
}

filter "configurations:Debug"
defines "TR_DEBUG"
runtime "Debug"
symbols "on"

filter "configurations:Release"
defines "TR_RELEASE"
runtime "Release"
optimize "on"

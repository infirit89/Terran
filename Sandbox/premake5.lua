VULKAN_SDK = os.getenv "VULKAN_SDK"
print("VULKAN_SDK = " .. VULKAN_SDK)

project "Sandbox"
pic "On"
kind "ConsoleApp"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir("%{prj.location}/bin/" .. outputdir)
objdir("%{prj.location}/bin-int/" .. outputdir)

debuggertype "NativeWithManagedCore"

files {
    "src/**.h",
    "src/**.cpp",
}

includedirs {
    "src",
    "%{wks.location}/TerranEngine/src/",
    "%{wks.location}/Libraries/",
    "%{Dependencies.spdlog.include}",
    "%{IncludeDirectories.imgui}",
    "%{Dependencies.glm.include}",
    "%{IncludeDirectories.entt}",
    "%{IncludeDirectories.imguizmo}",
    "%{IncludeDirectories.optick}",
    "%{wks.location}/TerranEditor/vendor/FontAwesome/",
}

links {
    "TerranEngine",
    "LibCore",
}

defines {
    "_CRT_SECURE_NO_WARNINGS",
}

CopyCommands = {}
CopyCommands["optick"] = "{COPY} %{ExternalLibraries.optick} %{prj.location}/bin/%{outputdir}"
CopyCommands["shaderc"] = "{COPY} %{ExternalLibraries.shaderc_shared} %{prj.location}/bin/%{outputdir}"
CopyCommands["coral"] = "{COPY} %{External.coral} %{prj.location}/Resources/Scripts"
CopyCommands["script_core"] = "{COPY} %{InternalLibraries.script_core} %{prj.location}/Resources/Scripts"

filter "system:windows"
architecture "x86_64"
systemversion "latest"

postbuildcommands {
    -- todo: copy the pdb
    "%{CopyCommands.optick}",
    "%{CopyCommands.shaderc}",
    "%{CopyCommands.coral}",
    "{MKDIR} %{prj.location}/Resources/Scripts",
    "%{CopyCommands.script_core}",
}

filter "system:macosx"
architecture "ARM64"

libdirs {
    "/usr/local/lib",
    "%{VULKAN_SDK}/lib",
    -- "%{wks.location}/TerranEngine/vendor/shaderc/lib"
}

links {
    "CoreFoundation.framework", -- no path needed for system frameworks
    "OpenGL.framework",
    "Cocoa.framework",
    "IOKit.framework",
    "QuartzCore.framework",
    "GLFW",
    "ImGui",
    "GLAD",
    "msdf-atlas-gen",
    "Box2D",
    "yaml-cpp",
    "Coral.Native",
    "msdfgen",
    "freetype",
    "shaderc_shared",
}

postbuildcommands {
    -- todo: copy the pdb
    "{MKDIR} %{prj.location}/Resources/Scripts",
    "%{CopyCommands.coral}",
    "%{CopyCommands.script_core}",
}

defines "TR_SANDBOX"

filter "configurations:Debug"
defines "TR_DEBUG"
runtime "Debug"
symbols "on"

filter "configurations:Release"
defines "TR_RELEASE"
runtime "Release"
optimize "on"

-- todo: copy in release mode

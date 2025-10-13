project "LibCore"
pic "On"
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir("%{prj.location}/bin/" .. outputdir)
objdir("%{prj.location}/bin-int/" .. outputdir)

files {
    "**.h",
    "**.cpp",

    "vendor/glm/glm/**.hpp",
    "vendor/glm/glm/**.inl",
}

includedirs {
    "%{Dependencies.spdlog.include}",
    "%{Dependencies.glm.include}",
    "%{Dependencies.yaml.include}",
}

linkoptions {
    "-IGNORE:4006",
}

filter "system:macosx"
architecture "ARM64"

libdirs { "/usr/local/lib" }
links {
    "CoreFoundation.framework", -- no path needed for system frameworks
}

filter "system:windows"
architecture "x86_64"
systemversion "latest"

defines {
    "_CRT_SECURE_NO_WARNINGS",
}

includedirs {
    "%{IncludeDirectories.optick}",
}

links {
    "OptickCore",
}

filter "configurations:Debug"
defines "TR_DEBUG"
runtime "Debug"
symbols "on"

filter "configurations:Release"
defines "TR_RELEASE"
runtime "Release"
optimize "on"

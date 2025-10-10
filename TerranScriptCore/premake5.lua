project "TerranScriptCore"
kind "SharedLib"
language "C#"
dotnetframework "net8.0"
clr "Unsafe"

propertytags {
    { "AppendTargetFrameworkToOutputPath", "false" },
    { "Nullable", "enable" },
}

targetdir "Build/%{cfg.buildcfg}"
objdir "Intermidiates/%{cfg.buildcfg}"

files {
    "Source/**.cs",
}

includedirs {
    "Source",
}

links {
    "Coral.Managed",
}

filter "system:windows"
systemversion "latest"

filter "configurations:Debug"
defines "DEBUG"
runtime "Debug"
symbols "on"

filter "configurations:Release"
defines "RELEASE"
runtime "Release"
optimize "on"

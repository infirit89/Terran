workspace "SandboxProject"
    architecture "x86_64"
    
    configurations 
    {
        "Debug",
        "Release"
    }

project "SandboxProject"
    kind "SharedLib"
    language "C#"

    targetdir ("../Resources/Scripts/")
    objdir ("../Resources/Scripts/Intermidiates")

    files 
    {
        "Source/**.cs"
    }

    includedirs
    {
        "Source"
    } 
    
    links 
    {
        "../Resources/Scripts/TerranScriptCore"
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
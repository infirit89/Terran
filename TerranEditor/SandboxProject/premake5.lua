workspace "SandboxProject"
    architecture "x86_64"
    
    configurations 
    {
        "Debug",
        "Release"
    }

project "ScriptAssembly"
    kind "SharedLib"
    language "C#"

    targetdir ("ScriptAssemblies")
    objdir ("ScriptAssemblies/Intermidiates")

    files 
    {
        "Assets/**.cs"
    }

    includedirs
    {
        "Assets"
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

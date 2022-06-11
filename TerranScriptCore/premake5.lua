project "TerranScriptCore"
    kind "SharedLib"
    language "C#"

    targetdir ("../TerranEditor/Resources/Scripts")
    objdir ("../TerranEditor//Resources/Scripts/Intermidiates")

    files 
    {
        "Source/**.cs",
        "Properties/**.cs"
    }

    includedirs 
    {
        "Source",
        "Properties"
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
        
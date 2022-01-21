project "TerranScriptCore"
    kind "SharedLib"
    language "C#"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

    files 
    {
        "src/**.cs"   
    }

    includedirs 
    {
        "src"
    }

    links 
    {
        "System",
        "System.Core",
        "System.Numerics"

        -- todo: add more links if needed
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
 
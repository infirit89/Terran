project "TerranScriptCore"
    kind "SharedLib"
    language "C#"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

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
        "System",
        "System.Core",
        "System.Runtime",
        "System.Numerics"

        -- todo: add more links if needed
    }

    filter "system:windows"
        systemversion "latest"
        postbuildcommands  
        {
            ("{COPY} %{prj.location}/bin/Debug-%{cfg.system}-%{cfg.architecture} %{wks.location}/TerranEditor/Resources/Scripts/Temp")
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"
        
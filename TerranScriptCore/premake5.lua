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

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"
        
        postbuildcommands  
        {
            "{COPY} %{prj.location}/bin/" .. outputdir .. "/TerranScriptCore.dll %{wks.location}/TerranEditor/Resources/Scripts/Temp"
        }

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"

        postbuildcommands  
        {
            "{COPY} %{prj.location}/bin/" .. outputdir .. "/TerranScriptCore.dll %{wks.location}/TerranEditor/Resources/Scripts/Temp"
        }


 
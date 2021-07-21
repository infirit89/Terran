project "Freetype-GL"    
    language "C"
    kind "StaticLib"
    staticruntime "on"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

    files 
    {
        "src/**.c",
        "src/**.h",
    }

    includedirs 
    {
        "%{wks.location}/TerranEngine/vendor/freetype/include/",
        "%{wks.location}/TerranEngine/vendor/GLAD/include/"
    }

    links 
    {
        "GLAD",
        "opengl32.lib",
        "Freetype"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"
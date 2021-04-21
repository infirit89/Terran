project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{prj.location}/bin/" .. outputdir)
    objdir ("%{prj.location}/bin-int/" .. outputdir)

    files 
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "%{wks.location}/TerranEngine/src",
        "%{wks.location}/TerranEngine/vendor/spdlog/include",
        "%{wks.location}/TerranEngine/vendor/",
        "%{wks.location}/TerranEngine/vendor/ImGui/",
        "%{wks.location}/TerranEngine/vendor/glm/",
        "%{wks.location}/TerranEngine/vendor/entt/include"
    } 

    links 
    {
        "TerranEngine"
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
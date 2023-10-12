project "TerranEditor"
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

        "vendor/ImGuizmo/ImGuizmo.h",
        "vendor/ImGuizmo/ImGuizmo.cpp"
    }

    includedirs
    {
        "src",
        "%{wks.location}/TerranEngine/src/",
        "%{IncludeDirectories.spdlog}",
        "%{IncludeDirectories.imgui}",
        "%{IncludeDirectories.glm}",
        "%{IncludeDirectories.entt}",
        
        "%{IncludeDirectories.imguizmo}",
        "%{wks.location}/TerranEditor/vendor/FontAwesome/",
    } 
    
    links 
    {
        "TerranEngine"
    }

    defines 
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    CopyCommands = {}
    CopyCommands["mono"] = "{COPY} %{Libraries.mono_shared} %{prj.location}/bin/%{outputdir}"
    CopyCommands["optick"] = "{COPY} %{Libraries.optick} %{prj.location}/bin/%{outputdir}"
    CopyCommands["shaderc"] = "{COPY} %{Libraries.shaderc_shared} %{prj.location}/bin/%{outputdir}"

    filter "system:windows"
        systemversion "latest"
        
         postbuildcommands  
         {
             -- todo: copy the pdb
             "%{CopyCommands.mono}",
             "%{CopyCommands.optick}",
             "%{CopyCommands.shaderc}"
         }

    filter "configurations:Debug"
        defines "TR_DEBUG"
        runtime "Debug"
        symbols "on"


    filter "configurations:Release"
        defines "TR_RELEASE"
        runtime "Release"
        optimize "on"

        -- todo: copy in release mode

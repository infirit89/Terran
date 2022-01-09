mono_path = os.getenv("MONO_PATH")
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
        "%{wks.location}/TerranEngine/src/",
        "%{wks.location}/TerranEngine/vendor/",
        "%{wks.location}/TerranEngine/vendor/spdlog/include/",
        "%{wks.location}/TerranEngine/vendor/ImGui/",
        "%{wks.location}/TerranEngine/vendor/glm/",
        "%{wks.location}/TerranEngine/vendor/entt/include/",
        "%{wks.location}/TerranEngine/vendor/freetype-gl/src/",
        
        "%{wks.location}/TerranEditor/vendor/ImGuizmo/",
        "%{mono_path}/include/mono-2.0/"
    } 

    libdirs 
    {
        "%{mono_path}/lib/"
    }

    links 
    {
        "TerranEngine",
        "%{mono_path}/lib/mono-2.0-sgen.lib"
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
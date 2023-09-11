workspace "Terran"

    architecture "x86_64"
    startproject "TerranEditor"
    
    configurations 
    {
        "Debug",
        "Release"
    }

    flags 
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "TerranEngine/vendor/GLFW"
    include "TerranEngine/vendor/ImGui"
    include "TerranEngine/vendor/GLAD"
    include "TerranEngine/vendor/msdf-atlas-gen"
    include "TerranEngine/vendor/msdf-atlas-gen/msdfgen"
    include "TerranEngine/vendor/msdf-atlas-gen/msdfgen/Freetype"
    --include "TerranEngine/vendor/Freetype-GL"
    include "TerranEngine/vendor/Box2D"
    include "TerranEngine/vendor/Optick"
    include "TerranEngine/vendor/yaml-cpp"

group "Core"
    include "TerranEngine"
    include "TerranScriptCore"

group "Tools"
    include "TerranEditor"

group "Runtime"
    include "Sandbox"
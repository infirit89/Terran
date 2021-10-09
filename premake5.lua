workspace "Terran"

    architecture "x86_64"
    startproject "TerranEditor"
    configurations 
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
    include "TerranEngine/vendor/GLFW"
    include "TerranEngine/vendor/ImGui"
    include "TerranEngine/vendor/GLAD"
    include "TerranEngine/vendor/Freetype"
    include "TerranEngine/vendor/Freetype-GL"

group ""

include "TerranEngine"
include "Sandbox"
include "TerranEditor"
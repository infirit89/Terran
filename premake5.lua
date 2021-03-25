workspace "Terran"

    architecture "x86_64"
    startproject "Sandbox"
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

group ""

include "TerranEngine"
include "Sandbox"
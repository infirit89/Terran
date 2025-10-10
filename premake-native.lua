require "vendor/premake-vscode"
--require "extensions/vscode"
require "dependecies"

workspace "Terran"

startproject "TerranEditor"

configurations {
    "Debug",
    "Release",
}

flags {
    "MultiProcessorCompile",
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
include "TerranEngine/vendor/GLFW"
include "TerranEngine/vendor/ImGui"
include "TerranEngine/vendor/GLAD"
include "TerranEngine/vendor/msdf-atlas-gen"
include "TerranEngine/vendor/msdf-atlas-gen/msdfgen"
include "TerranEngine/vendor/msdf-atlas-gen/msdfgen/Freetype"
include "TerranEngine/vendor/Box2D"
-- include "TerranEngine/vendor/Optick" Think about replacing with Tracy
include "TerranEngine/vendor/yaml-cpp"
include "TerranEngine/vendor/Coral/Coral.Native"

group "Core"
include "TerranEngine"

group "Tools"
include "TerranEditor"

group "Runtime"
include "Sandbox"

require "vendor/premake-vscode"
--require "extensions/vscode"
require "dependecies"

workspace "Terran"

--  NOTE: for now setting the startproj to be Sandbox only for the rework
startproject "Sandbox"

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
include "vendor/yaml-cpp"
include "TerranEngine/vendor/Coral/Coral.Native"

group "Core"
include "Libraries/LibMain"
include "Libraries/LibCore"
include "TerranEngine"

group "Tools"
-- NOTE: removing the terran editor during the rework as it will cause
-- a lot of build errors and it can be dealt with separately
--include "TerranEditor"

group "Runtime"
include "Sandbox"

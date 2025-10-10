VULKAN_SDK = os.getenv "VULKAN_SDK"

IncludeDirectories = {}

IncludeDirectories["spdlog"] = "%{wks.location}/TerranEngine/vendor/spdlog/include/"
IncludeDirectories["glfw"] = "%{wks.location}/TerranEngine/vendor/GLFW/include/"
IncludeDirectories["imgui"] = "%{wks.location}/TerranEngine/vendor/ImGui/"
IncludeDirectories["glad"] = "%{wks.location}/TerranEngine/vendor/GLAD/include/"
IncludeDirectories["stb"] = "%{wks.location}/TerranEngine/vendor/stb/"
IncludeDirectories["glm"] = "%{wks.location}/TerranEngine/vendor/glm/"
IncludeDirectories["entt"] = "%{wks.location}/TerranEngine/vendor/entt/include/"
IncludeDirectories["msdfgen"] = "%{wks.location}/TerranEngine/vendor/msdf-atlas-gen/msdfgen/"
IncludeDirectories["msdf_atlas_gen"] = "%{wks.location}/TerranEngine/vendor/msdf-atlas-gen/"
IncludeDirectories["box2d"] = "%{wks.location}/TerranEngine/vendor/Box2D/include/"
IncludeDirectories["optick"] = "%{wks.location}/TerranEngine/vendor/Optick/src/"
IncludeDirectories["yaml"] = "%{wks.location}/TerranEngine/vendor/yaml-cpp/include/"
IncludeDirectories["imguizmo"] = "%{wks.location}/TerranEditor/vendor/ImGuizmo/"
IncludeDirectories["shaderc"] = "%{VULKAN_SDK}/include/"
-- IncludeDirectories["shaderc"] = "%{wks.location}/TerranEngine/vendor/shaderc/include/"
IncludeDirectories["coral"] = "%{wks.location}/TerranEngine/vendor/Coral/Coral.Native/Include/"

ExternalLibraries = {}
ExternalLibraries["optick"] = "%{wks.location}/TerranEngine/vendor/Optick/bin/%{outputdir}/OptickCore.dll"
ExternalLibraries["shaderc"] = "%{wks.location}/TerranEngine/vendor/shaderc/lib/shaderc_shared.lib"
ExternalLibraries["shaderc_shared"] = "%{wks.location}/TerranEngine/vendor/shaderc/bin/shaderc_shared.dll"
ExternalLibraries["imm32"] = "Imm32.lib"
ExternalLibraries["rpcrt4"] = "Rpcrt4.lib"

External = {}
External["coral"] = "%{wks.location}/TerranEngine/vendor/Coral/Coral.Managed/Coral.Managed.runtimeconfig.json"

InternalLibraries = {}
InternalLibraries["script_core"] = "%{wks.location}/TerranScriptCore/Build/%{cfg.buildcfg}"

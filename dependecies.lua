local function getOsName()
    local binaryFormat = package.cpath:match("%p[\\|/]?%p(%a+)")
    if binaryFormat == "dll" then
        return "win32"
    end
    if binaryFormat == "so" then
        return "linux"
    end
    if binaryFormat == "dylib" then
        return "macos"
    end
end

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
IncludeDirectories["mono"] = "%{wks.location}/TerranEngine/vendor/mono/include/"
IncludeDirectories["box2d"] = "%{wks.location}/TerranEngine/vendor/Box2D/include/"
IncludeDirectories["optick"] = "%{wks.location}/TerranEngine/vendor/Optick/src/"
IncludeDirectories["yaml"] = "%{wks.location}/TerranEngine/vendor/yaml-cpp/include/"
IncludeDirectories["imguizmo"] = "%{wks.location}/TerranEditor/vendor/ImGuizmo/"
IncludeDirectories["shaderc"] = "%{wks.location}/TerranEngine/vendor/shaderc/include/"


Libraries = {}
local osName = getOsName()

if osName == "win32" then
    Libraries["mono_static"] = "%{wks.location}/TerranEngine/vendor/mono/bin/mono-2.0-sgen.lib"
    Libraries["mono_shared"] = "%{wks.location}/TerranEngine/vendor/mono/bin/mono-2.0-sgen.dll"
    Libraries["optick"] = "%{wks.location}/TerranEngine/vendor/Optick/bin/%{outputdir}/OptickCore.dll"
    Libraries["shaderc"] = "%{wks.location}/TerranEngine/vendor/shaderc/lib/shaderc_shared.lib"
    Libraries["shaderc_shared"] = "%{wks.location}/TerranEngine/vendor/shaderc/bin/shaderc_shared.dll"
    Libraries["imm32"] = "Imm32.lib"
    Libraries["rpcrt4"] = "Rpcrt4.lib"
elseif osName == "linux" then
    Libraries["mono_static"] = "%{wks.location}/TerranEngine/vendor/mono/bin/libmonosgen-2.0.so"
    Libraries["mono_shared"] = "%{wks.location}/TerranEngine/vendor/mono/bin/libmonosgen-2.0.so"
    Libraries["optick"] = "%{wks.location}/TerranEngine/vendor/Optick/bin/%{outputdir}/OptickCore.dll"
    Libraries["shaderc"] = "%{wks.location}/TerranEngine/vendor/shaderc/lib/libshaderc.a"
    Libraries["shaderc_shared"] = "%{wks.location}/TerranEngine/vendor/shaderc/bin/libshaderc_shared.so"
end

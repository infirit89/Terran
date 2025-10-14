#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/projection.hpp"

#include "SandboxLayer.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <imgui.h>

#include <math.h>

#include "LibCore/Log.h"

namespace TerranEngine {

static void TestFunc()
{
    for (size_t i = 0; i < 100; i++)
        TR_CLIENT_TRACE(i * (i + i));

    TR_CLIENT_ERROR("Error: something");

    TR_CLIENT_WARN("Some warning");
}

SandboxLayer::SandboxLayer()
    : Layer("Sandbox Layer")
{
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnDetach()
{
}

void SandboxLayer::Update(Terran::Core::Time& time)
{
}

void SandboxLayer::OnEvent(Terran::Core::Event& event)
{
    Terran::Core::EventDispatcher dispather(event);
}

void SandboxLayer::ImGuiRender()
{
}

}

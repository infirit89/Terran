#pragma once

/* ---- Base ---- */
#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Core/Base.h"

/* ---- Events ---- */
#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

/* ---- ImGui ---- */
#include "ImGui/ImGuiLayer.h"

/* ---- Graphics ---- */
#include "Renderer/Buffer.h"
#include "Scene/OrthographicCamera.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Texture.h"
#include "Renderer/BatchRenderer.h"
#include "Renderer/TextureSheet.h"

/* ---- Debugging ---- */
#include "Utils/Debug/DebugTimer.h"
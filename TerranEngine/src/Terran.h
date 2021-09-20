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
#include "Renderer/OrthographicCamera.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Texture.h"
#include "Renderer/BatchRenderer2D.h"
#include "Renderer/TextureSheet.h"
#include "Renderer/Font.h"

/* ---- Debugging ---- */
#include "Utils/Debug/DebugTimer.h"

/* ---- ECS ---- */
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
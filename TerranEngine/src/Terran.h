#pragma once

/* ---- Base ---- */
#include "Core/Application.h"
#include "Core/Log.h"
#include "Core/Layer.h"
#include "Core/Input.h"
#include "Core/FileUtils.h"
#include "Core/Base.h"
#include "Core/UUID.h"
#include "Core/Time.h"

/* ---- Events ---- */
#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Events/Event.h"

/* ---- Utils ---- */
//#include "Utils/FileUtils.h"

/* ---- ImGui ---- */
#include "ImGui/ImGuiLayer.h"

/* ---- Graphics ---- */
#include "Graphics/Buffer.h"
#include "Graphics/OrthographicCamera.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/RenderCommand.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer2D.h"
#include "Graphics/TextureSheet.h"
#include "Graphics/Font.h"
#include "Graphics/Camera.h"

/* ---- Debugging ---- */
#include "Utils/Debug/Profiler.h"

/* ---- Scene ---- */
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"

#include "Scene/Systems/SceneRenderer.h"

/* ---- Misc ---- */
#include "Math/Math.h"
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
#include "Events/ScriptEngineEvent.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/MouseEvent.h"
#include "Events/GamepadEvent.h"
#include "Events/Event.h"

/* ---- ImGui ---- */
#include "ImGui/ImGuiLayer.h"

/* ---- Graphics ---- */
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/UniformBuffer.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/OrthographicCamera.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/Renderer.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer2D.h"
#include "Graphics/TextureSheet.h"
#include "Graphics/Camera.h"
#include "Graphics/Font.h"

/* ---- Debugging ---- */
#include "Utils/Debug/Profiler.h"

/* ---- Scene ---- */
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/SceneSerializer.h"
#include "Scene/SceneManager.h"

#include "Scene/Systems/SceneRenderer.h"

/* ---- Misc ---- */
#include "Math/Math.h"

/* ---- Scripting ---- */
#include "Scripting/ScriptEngine.h"

/* ---- Physics ---- */
#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/Collider.h"
#include "Physics/PhysicsLayerManager.h"

/* ---- Assets ---- */
#include "Asset/Asset.h"
#include "Asset/AssetManager.h"
#include "Asset/AssetImporter.h"

/* ---- Project ---- */
#include "Project/Project.h"
#include "Project/ProjectSerializer.h"


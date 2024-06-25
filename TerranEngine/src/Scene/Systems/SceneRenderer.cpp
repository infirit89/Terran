#include "trpch.h"
#include "SceneRenderer.h"

#include "Scene/Entity.h"

#include "Core/Input.h"

#include "Graphics/BatchRenderer2D.h"
#include "Graphics/Renderer.h"

#include "Asset/AssetManager.h"

#include "Math/Math.h"

#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	SceneRenderer::SceneRenderer(FramebufferParameters params)
	{
		m_Framebuffer = CreateShared<Framebuffer>(params);
	}

	void SceneRenderer::SetScene(Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneRenderer::BeginScene(Camera& camera, glm::mat4& cameraTransform, bool invereTransform)
	{
		m_Framebuffer->Bind();
		
		Renderer::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
		Renderer::Clear();

		BatchRenderer2D::BeginFrame(camera, cameraTransform, invereTransform);
		m_BegunScene = true;
		
		/* TODO: better sorting
		*  also add circle sorting
		*/

		m_Scene->m_Registry.sort<SpriteRendererComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.ZIndex < rEntity.ZIndex; });
	}

	void SceneRenderer::SubmitSprite(SpriteRendererComponent& spriteRenderer, glm::mat4& transform, int entityID)
	{
		// TODO: frustum culling
		Shared<Texture2D> texture = AssetManager::GetAsset<Texture2D>(spriteRenderer.TextureHandle);
		BatchRenderer2D::AddQuad(transform, spriteRenderer.Color, texture, entityID);
	}

	void SceneRenderer::SubmitCircle(CircleRendererComponent& circleRenderer, glm::mat4& transform, int entityID)
	{
		// TODO: frustum culling
		BatchRenderer2D::AddCircle(transform, circleRenderer.Color, circleRenderer.Thickness, entityID);
	}

	void SceneRenderer::SubmitLine(LineRendererComponent& lineRenderer, int entityID)
	{
		BatchRenderer2D::AddLine(lineRenderer.StartPoint, lineRenderer.EndPoint, lineRenderer.Color, lineRenderer.Thickness, entityID);
	}

	void SceneRenderer::SubmitText(TextRendererComponent& textRenderer, glm::mat4& transform, int entityID)
	{
		BatchRenderer2D::AddString(transform, textRenderer.Text, textRenderer.TextColor, textRenderer.FontAtlas, 
										textRenderer.LineSpacing, textRenderer.LineWidth, entityID);
	}

	void SceneRenderer::EndScene()
	{
		// TODO: draw grid
		if (m_ShowColliders)
			SubmitColliderBounds();

		TR_ASSERT(m_BegunScene, "BeginScene has to be called before EndScene!");

		BatchRenderer2D::EndFrame();
		m_Framebuffer->Unbind();

		m_BegunScene = false;
	}

	void SceneRenderer::OnResize(uint32_t width, uint32_t height)
	{
		if(m_Framebuffer->GetWidth() != width || m_Framebuffer->GetHeight() != height)
			m_Framebuffer->Resize(width, height);
	}

	void SceneRenderer::SubmitColliderBounds()
	{
		// submit box collider bounds
		{
			auto boxColliderView = m_Scene->GetEntitiesWith<BoxCollider2DComponent>();
			for (auto e: boxColliderView)
			{
				Entity entity(e, m_Scene);

				BoxCollider2DComponent& boxCollider = entity.GetComponent<BoxCollider2DComponent>();
				auto& transform = entity.GetTransform();

				const glm::vec4 color = { 0.0f, 1.0f, 0.0f, 1.0f };
				const float thickness = 0.05f;

				glm::vec3 size = { boxCollider.Size.x, boxCollider.Size.y, 1.0f };

				glm::vec3 postition = { boxCollider.Offset.x, boxCollider.Offset.y, 1.0f };

				glm::mat4 worldTransformMatrix = transform.WorldSpaceTransformMatrix;
				glm::mat4 transformMatrix = worldTransformMatrix *
											glm::translate(glm::mat4(1.0f), postition) *
											glm::scale(glm::mat4(1.0f), size);

				/*const glm::vec3 size = { transform.Scale.x * boxCollider.Size.x, transform.Scale.y * boxCollider.Size.y, 1.0f };

				const glm::vec3 postition = { transform.Position.x + boxCollider.Offset.x, transform.Position.y + boxCollider.Offset.y, 1.0f };

				glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), postition) *
					glm::rotate(glm::mat4(1.0f), transform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::scale(glm::mat4(1.0f), size);*/

				BatchRenderer2D::AddDebugRect(transformMatrix, color);
			}
		}

		// submit circle collider bounds
		{
			auto circleColliderView = m_Scene->GetEntitiesWith<CircleCollider2DComponent>();

			for (auto e : circleColliderView)
			{
				Entity entity(e, m_Scene);

				auto& circleCollider = entity.GetComponent<CircleCollider2DComponent>();
				auto& transform = entity.GetTransform();

				const glm::vec4 color = { 0.0f, 1.0f, 0.0f, 1.0f };
				const float thickness = 0.02f;

				glm::vec3 position, rotation, scale;

				Math::Decompose(transform.WorldSpaceTransformMatrix, position, rotation, scale);

				// choose which component of the scale to apply
				float scalingFactor = scale.x > scale.y ? scale.x : scale.y;

				glm::vec3 colliderSize = scalingFactor * glm::vec3(circleCollider.Radius * 2.0f);
				glm::vec3 colliderPosition = { position.x + circleCollider.Offset.x, 
												position.y + circleCollider.Offset.y, 1.0f };

				glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), colliderPosition) * 
											glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) * 
											glm::scale(glm::mat4(1.0f), colliderSize);

				BatchRenderer2D::AddCircle(transformMatrix, color, thickness, -1);
			}
		}

		// submit capsule collider bounds
		{
			auto capsuleColliderView = m_Scene->GetEntitiesWith<CapsuleCollider2DComponent>();

			for (auto e : capsuleColliderView)
			{
				Entity entity(e, m_Scene);

				auto& capsuleCollider = entity.GetComponent<CapsuleCollider2DComponent>();
				auto& transform = entity.GetTransform();

				const glm::vec4 color = { 0.0f, 1.0f, 0.0f, 1.0f };
				const float thickness = 0.02f;

				float ySize = capsuleCollider.Size.x > capsuleCollider.Size.y ? capsuleCollider.Size.x : capsuleCollider.Size.y;
				const glm::vec3 size = { capsuleCollider.Size.x, ySize, 1.0f };
				const glm::vec3 position = { capsuleCollider.Offset.x, capsuleCollider.Offset.y, 1.0f };

				glm::mat4 transformMatrix = transform.WorldSpaceTransformMatrix * 
											glm::translate(glm::mat4(1.0f), position) *
											glm::scale(glm::mat4(1.0f), size);

				BatchRenderer2D::AddCircle(transformMatrix, color, thickness, -1);
			}
		}
	}
}

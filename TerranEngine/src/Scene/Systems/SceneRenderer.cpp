#include "trpch.h"
#include "SceneRenderer.h"

#include "Scene/Entity.h"

#include "Core/Input.h"

#include "Graphics/BatchRenderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

namespace TerranEngine 
{
	SceneRenderer::SceneRenderer()
	{
		m_Framebuffer = CreateUnique<Framebuffer>(m_Width, m_Height);
	}

	void SceneRenderer::SetScene(Scene* scene)
	{
		m_Scene = scene;
	}

	void SceneRenderer::BeginScene(Camera& camera, glm::mat4& cameraTransform, bool invereTransform)
	{
		m_Framebuffer->Bind();
		
		RenderCommand::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
		RenderCommand::Clear();

		BatchRenderer2D::Get()->BeginFrame(camera, cameraTransform, invereTransform);
		m_BegunScene = true;

		/* TODO: better sorting
		*  also add circle sorting
		*/

		m_Scene->m_Registry.sort<SpriteRendererComponent>([](const auto& lEntity, const auto& rEntity) 
		{ return lEntity.ZIndex < rEntity.ZIndex; });
	}

	void SceneRenderer::SubmitSprite(SpriteRendererComponent& spriteRenderer, glm::mat4& transform)
	{
		// TODO: frustum culling
		BatchRenderer2D::Get()->AddQuad(transform, spriteRenderer.Color, spriteRenderer.Texture);
	}

	void SceneRenderer::SubmitCircle(CircleRendererComponent& circleRenderer, glm::mat4& transform)
	{
		// TODO: frustum culling
		BatchRenderer2D::Get()->AddCircle(transform, circleRenderer.Color, circleRenderer.Thickness);
	}

	void SceneRenderer::SubmitLine(LineRendererComponent& lineRenderer)
	{
		BatchRenderer2D::Get()->AddLine(lineRenderer.Points, lineRenderer.PointCount, lineRenderer.Color, lineRenderer.Thickness);
	}

	void SceneRenderer::SubmitText(TextRendererComponent& textRenderer, glm::mat4& transform)
	{
		BatchRenderer2D::Get()->AddText(transform, "", textRenderer.TextColor, textRenderer.FontAtlas);
	}

	void SceneRenderer::SubmitDebugRectangle(const glm::mat4& transform, const glm::vec4& color, float thickness)
	{
		BatchRenderer2D::Get()->AddRect(transform, color, thickness);
	}

	void SceneRenderer::EndScene()
	{
		// TODO: draw grid
		if (m_ShowColliders)
			SubmitColliderBounds();

		TR_ASSERT(m_BegunScene, "BeginScene has to be called before EndScene!");

		BatchRenderer2D::Get()->EndFrame();
		m_Framebuffer->Unbind();

		m_BegunScene = false;

		RenderCommand::WireframeMode(false);
	}

	void SceneRenderer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_Width != width || m_Height != height) 
		{
			m_Width = width; m_Height = height;
			m_Framebuffer->Resize(m_Width, m_Height);
		}
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
				const float thickness = 0.02f;

				const glm::vec3 size = { transform.Scale.x * boxCollider.Size.x, transform.Scale.y * boxCollider.Size.y, 1.0f };

				const glm::vec3 postition = { transform.Position.x + boxCollider.Offset.x, transform.Position.y + boxCollider.Offset.y, 1.0f };

				glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), postition) *
					glm::rotate(glm::mat4(1.0f), transform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::scale(glm::mat4(1.0f), size);

				SubmitDebugRectangle(transformMatrix, color, thickness);
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

				const glm::vec3 size = transform.Scale * circleCollider.Radius;
				const glm::vec3 position = { transform.Position.x + circleCollider.Offset.x, transform.Position.y + circleCollider.Offset.y, 1.0f };

				glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), position) *
					glm::rotate(glm::mat4(1.0f), transform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::scale(glm::mat4(1.0f), size);

				CircleRendererComponent circleRenderer;
				circleRenderer.Color = color;
				circleRenderer.Thickness = thickness;

				SubmitCircle(circleRenderer, transformMatrix);
			}
		}
	}
}

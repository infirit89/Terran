#include "trpch.h"
#include "SceneRenderer.h"

#include "Graphics/BatchRenderer2D.h"

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

	void SceneRenderer::BeginScene(Camera& camera, glm::mat4& cameraTransform)
	{
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f);
		RenderCommand::Clear();

		BatchRenderer2D::Get()->BeginFrame(camera, cameraTransform);
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

	void SceneRenderer::EndScene()
	{
		// TODO: draw grid

		TR_ASSERT(m_BegunScene, "BeginScene has to be called before EndScene!");

		BatchRenderer2D::Get()->EndFrame();
		m_Framebuffer->Unbind();

		m_BegunScene = false;
	}

	void SceneRenderer::OnResize(uint32_t width, uint32_t height)
	{
		if (m_Width != width || m_Height != height) 
		{
			m_Width = width; m_Height = height;
			m_Framebuffer->Resize(m_Width, m_Height);
		}
	}
}

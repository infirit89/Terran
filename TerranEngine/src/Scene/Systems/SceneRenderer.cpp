#include "trpch.h"
#include "SceneRenderer.h"

#include "Core/Input.h"

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

	void SceneRenderer::BeginScene(Camera& camera, glm::mat4& cameraTransform, bool invereTransform)
	{
		m_Framebuffer->Bind();
		
		if(Input::IsKeyPressed(Key::F))
			RenderCommand::WireframeMode(true);

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
		BatchRenderer2D::Get()->AddLine(lineRenderer.Point1, lineRenderer.Point2, lineRenderer.Color, lineRenderer.Thickness);
	}

	void SceneRenderer::SubmitDebugRectangle(const glm::mat4& transform, const glm::vec4& color, float thickness)
	{
		BatchRenderer2D::Get()->AddRect(transform, color, thickness);
	}

	void SceneRenderer::EndScene()
	{
		// TODO: draw grid

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
}

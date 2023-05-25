#include "trpch.h"
#include "BatchRenderer2D.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "Framebuffer.h"
#include "RenderCommand.h"

#include "Math/Math.h"

#include "Utils/Debug/OptickProfiler.h"

#include <glad/glad.h>

namespace TerranEngine 
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoordinates;
		int TextureIndex;
		int EntityID;
	};

	struct CircleVertex
	{
		glm::vec3 Position;
		float Thickness;
		glm::vec4 Color;
		glm::vec2 LocalPosition;
		int EntityID;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec3 PositionA;
		glm::vec3 PositionB;
		glm::vec4 Color;
		float Thickness;
		int EntityID;
	};

	struct DebugLineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct TextVertex
	{
		glm::vec3 Position;
		int TextureIndex;
		glm::vec4 TextColor;
		glm::vec2 TextureCoordinates;
		int EntityID;
	};

	struct CameraData
	{
		glm::mat4 Projection;
		glm::mat4 View;
	};

	struct BatchRenderer2DData 
	{
		// ******** Base stuffs ********
		static BatchRenderer2D* Instance;
		BatchRendererStats Stats;
		glm::vec4 VertexPositions[4];
		glm::vec4 LineVertexPositions[4];
		uint32_t MaxVertices, MaxIndices;

		static const uint32_t MaxTextureSlots = 16;
		// *****************************

		Shared<IndexBuffer>  IndexBuffer;

		// ******** Quad ********
		uint32_t QuadIndexCount = 0;
		Shared<VertexArray>  QuadVertexArray;
		Shared<VertexBuffer> QuadVertexBuffer;

		Shared<Shader> QuadShader;

		QuadVertex* QuadVertexPtr = nullptr;
		uint32_t QuadVertexPtrIndex = 0;

		Shared<Texture> QuadTextures[MaxTextureSlots];
		uint32_t QuadTextureIndex = 1;
		// **********************

		// ******** Circle ********
		uint32_t CircleIndexCount = 0;
		Shared<VertexArray>  CircleVertexArray;
		Shared<VertexBuffer> CircleVertexBuffer;

		Shared<Shader> CircleShader;

		CircleVertex* CircleVertexPtr = nullptr;
		uint32_t CircleVertexPtrIndex = 0;
		// ************************

		// NOTE: not so shitty anymore; still a bit bad
		// ******** Line ******** 
		uint32_t LineIndexCount = 0;
		Shared<Shader> LineShader;

		Shared<VertexArray> LineVertexArray;
		Shared<VertexBuffer> LineVertexBuffer;

		LineVertex* LineVertexPtr = nullptr;
		uint32_t LineVertexPtrIndex = 0;
		// **********************

		// ******** Debug Line ********
		Shared<Shader> DebugLineShader;

		Shared<VertexArray> DebugLineVertexArray;
		Shared<VertexBuffer> DebugLineVertexBuffer;

		DebugLineVertex* DebugLineVertexPtr = nullptr;
		uint32_t DebugLineVertexPtrIndex = 0;
		// ****************************

		// ******** Text ********
		uint32_t TextIndexCount = 0;
		Shared<VertexBuffer> TextVertexBuffer;
		Shared<VertexArray> TextVertexArray;

		Shared<Shader> TextShader;

		TextVertex* TextVertexPtr = nullptr;
		uint32_t TextVertexPtrIndex = 0;

		Shared<Texture> TextTextures[MaxTextureSlots];
		uint32_t TextTextureIndex = 0;

		// **********************

		//  ******** Camera stuffs ********


		CameraData CameraData;

		Shared<UniformBuffer> CameraBuffer;
	};

	BatchRenderer2DData* s_Data;

	void BatchRenderer2D::Initialize(uint32_t batchSize)
	{
		s_Data = new BatchRenderer2DData();

		s_Data->MaxIndices = batchSize * 6;
		s_Data->MaxVertices = batchSize * 4;

		s_Data->Stats.MaxIndices = s_Data->MaxIndices;
		s_Data->Stats.MaxVertices = s_Data->MaxVertices;

		// ******** Index buffer setup ********
		int* indices = new int[s_Data->MaxIndices];

		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			indices[i] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		s_Data->IndexBuffer = IndexBuffer::Create(indices, s_Data->MaxIndices * sizeof(int));

		delete[] indices;
		// ************************************

		constexpr uint32_t whiteTextureData = 0xffffffff;

		int samplers[16];
		for (size_t i = 0; i < s_Data->MaxTextureSlots; i++)
			samplers[i] = i;

		// ******** Quad ******** 
		{
			s_Data->QuadVertexPtr = new QuadVertex[s_Data->MaxVertices];

			s_Data->QuadVertexArray = VertexArray::Create();

			s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(QuadVertex));

			s_Data->QuadVertexArray->AddVertexBuffer(
			s_Data->QuadVertexBuffer,
			{
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Float,	4 },
				{ ShaderDataType::Float,	2 },
				{ ShaderDataType::Int,		1 },
				{ ShaderDataType::Int,		1 }
			});

			s_Data->QuadVertexArray->AddIndexBuffer(s_Data->IndexBuffer);

			s_Data->QuadShader = CreateShared<Shader>("DefaultQuadShader", 
												"Resources/Shaders/Base/Quad/QuadVertex.glsl", 
												"Resources/Shaders/Base/Quad/QuadFragment.glsl");

			//s_Data->QuadShader->Bind();
			s_Data->QuadShader->UploadIntArray("u_Samplers", s_Data->MaxTextureSlots, samplers);
			s_Data->QuadShader->Unbind();

			s_Data->QuadTextures[0] = CreateShared<Texture>(1, 1);
			s_Data->QuadTextures[0]->SetData(&whiteTextureData);

		}
		// **********************

		// ******** Circle ********
		{
			s_Data->CircleVertexPtr = new CircleVertex[s_Data->MaxVertices];

			s_Data->CircleVertexArray = VertexArray::Create();

			s_Data->CircleVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(CircleVertex));

			s_Data->CircleVertexArray->AddVertexBuffer(
			s_Data->CircleVertexBuffer,
			{
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Float,	1 },
				{ ShaderDataType::Float,	4 },
				{ ShaderDataType::Float,	2 },
				{ ShaderDataType::Int,		1 }
			});

			s_Data->CircleVertexArray->AddIndexBuffer(s_Data->IndexBuffer);

			s_Data->CircleShader = CreateShared<Shader>(	
													"DefaultCircleShader", 
													"Resources/Shaders/Base/Circle/CircleVertex.glsl",
													"Resources/Shaders/Base/Circle/CircleFragment.glsl");
		}
		// ************************

		// ******** Line ******** 
		{
			s_Data->LineVertexPtr = new LineVertex[s_Data->MaxVertices];
			s_Data->LineVertexArray = VertexArray::Create();
			s_Data->LineVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(LineVertex));

			s_Data->LineVertexArray->AddVertexBuffer(
			s_Data->LineVertexBuffer,
			{
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Float,	4 },
				{ ShaderDataType::Float,	1 },
				{ ShaderDataType::Int,		1 }
			});

			s_Data->LineVertexArray->AddIndexBuffer(s_Data->IndexBuffer);

			s_Data->LineShader = CreateShared<Shader>("DefaultLineShader", "Resources/Shaders/Base/Line/LineVertex.glsl", "Resources/Shaders/Base/Line/LineFragment.glsl");
		}
		// **********************

		// ******** Text ******** 
		{
			s_Data->TextVertexPtr = new TextVertex[s_Data->MaxVertices];
			s_Data->TextVertexArray = VertexArray::Create();
			s_Data->TextVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(TextVertex));

			s_Data->TextVertexArray->AddVertexBuffer(
			s_Data->TextVertexBuffer,
			{
				{ ShaderDataType::Float,	3 },
				{ ShaderDataType::Int,		1 },
				{ ShaderDataType::Float,	4 },
				{ ShaderDataType::Float,	2 },
				{ ShaderDataType::Int,		1 },
			});

			s_Data->TextVertexArray->AddIndexBuffer(s_Data->IndexBuffer);

			s_Data->TextShader = CreateShared<Shader>("DefaultTextShader", "Resources/Shaders/Base/Text/TextVertex.glsl", "Resources/Shaders/Base/Text/TextFragment.glsl");
			s_Data->TextShader->Bind();
			s_Data->TextShader->UploadIntArray("u_Samplers", s_Data->MaxTextureSlots, samplers);
			s_Data->TextShader->Unbind();
		}
		// **********************

		// ******** Debug Line ******** 
		{
			s_Data->DebugLineVertexPtr = new DebugLineVertex[s_Data->MaxVertices];
			s_Data->DebugLineVertexArray = VertexArray::Create();
			s_Data->DebugLineVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(DebugLineVertex));

			s_Data->DebugLineVertexArray->AddVertexBuffer(
			s_Data->DebugLineVertexBuffer,
			{
				{ ShaderDataType::Float, 3 },
				{ ShaderDataType::Float, 4 }
			});

			s_Data->DebugLineShader = CreateShared<Shader>("DefaultTextShader", "Resources/Shaders/Debug/Line/LineVertex.glsl", "Resources/Shaders/Debug/Line/LineFragment.glsl");
		}
		// ****************************

		s_Data->VertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->VertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->VertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		s_Data->LineVertexPositions[0] = {  0.0f, -0.5f, 0.0f, 1.0f };
		s_Data->LineVertexPositions[1] = {  1.0f, -0.5f, 0.0f, 1.0f };
		s_Data->LineVertexPositions[2] = {  1.0f,  0.5f, 0.0f, 1.0f };
		s_Data->LineVertexPositions[3] = {  0.0f,  0.5f, 0.0f, 1.0f };

		s_Data->QuadShader->Bind();
		s_Data->CircleShader->Bind();
		s_Data->LineShader->Bind();
		s_Data->TextShader->Bind();
		s_Data->DebugLineShader->Bind();

		s_Data->CameraBuffer = CreateShared<UniformBuffer>(sizeof(CameraData), 0);

		s_Data->QuadShader->Unbind();
		s_Data->CircleShader->Unbind();
		s_Data->LineShader->Unbind();
		s_Data->TextShader->Unbind();
		s_Data->DebugLineShader->Unbind();
	}

	void BatchRenderer2D::Shutdown()
	{
		delete[] s_Data->QuadVertexPtr;
		delete[] s_Data->CircleVertexPtr;
		delete[] s_Data->LineVertexPtr;
		delete[] s_Data->TextVertexPtr;
		delete[] s_Data->DebugLineVertexPtr;
	}

	void BatchRenderer2D::BeginFrame(Camera& camera, const glm::mat4& transform, bool inverseView)
	{
		TR_PROFILE_FUNCTION();
		Clear();

		s_Data->QuadShader->Bind();
		s_Data->CircleShader->Bind();
		s_Data->LineShader->Bind();
		s_Data->TextShader->Bind();
		s_Data->DebugLineShader->Bind();

		s_Data->CameraData.Projection = camera.GetProjection();
		s_Data->CameraData.View = inverseView ? glm::inverse(transform) : transform;

		s_Data->CameraBuffer->SetData(&s_Data->CameraData, 0, sizeof(CameraData));

		s_Data->QuadShader->Unbind();
		s_Data->CircleShader->Unbind();
		s_Data->LineShader->Unbind();
		s_Data->TextShader->Unbind();
		s_Data->DebugLineShader->Unbind();
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, int entityID)
	{
		TR_PROFILE_FUNCTION();
		glm::vec2 textureCoords[4] = 
		{
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
		};

		AddQuad(transform, color, texture, textureCoords, entityID);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		TR_PROFILE_FUNCTION();
		AddQuad(transform, color, nullptr, entityID);
	}

	void BatchRenderer2D::AddQuad(glm::mat4& transform, const glm::vec4& color, Shared<Texture> texture, glm::vec2 textureCoordinates[4], int entityID)
	{
		TR_PROFILE_FUNCTION();
		if (!QuadBatchHasRoom()) 
		{
			// Begin New Batch
			EndFrame();
			Clear();
		}

		int texIndex = -1;

		if (texture == nullptr)
			texIndex = 0;

		for (size_t i = 1; i < s_Data->QuadTextureIndex; i++)
		{
			if (s_Data->QuadTextures[i] == texture)
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = s_Data->QuadTextureIndex;
			s_Data->QuadTextures[s_Data->QuadTextureIndex] = texture;
			s_Data->QuadTextureIndex++;
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexPtr[s_Data->QuadVertexPtrIndex].Position = transform * s_Data->VertexPositions[i];
			s_Data->QuadVertexPtr[s_Data->QuadVertexPtrIndex].Color = color;
			s_Data->QuadVertexPtr[s_Data->QuadVertexPtrIndex].TextureCoordinates = textureCoordinates[i];
			s_Data->QuadVertexPtr[s_Data->QuadVertexPtrIndex].TextureIndex = texIndex;
			s_Data->QuadVertexPtr[s_Data->QuadVertexPtrIndex].EntityID = entityID;

			s_Data->QuadVertexPtrIndex++;
		}

		s_Data->QuadIndexCount += 6;
	}

	void BatchRenderer2D::AddCircle(glm::mat4& transform, const glm::vec4& color, float thickness, int entityID)
	{
		TR_PROFILE_FUNCTION();
		if (!CircleBatchHasRoom())
		{
			// Begin New Batch
			EndFrame();
			Clear();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->CircleVertexPtr[s_Data->CircleVertexPtrIndex].Position = transform * s_Data->VertexPositions[i];
			s_Data->CircleVertexPtr[s_Data->CircleVertexPtrIndex].Thickness = thickness;
			s_Data->CircleVertexPtr[s_Data->CircleVertexPtrIndex].Color = color;
			s_Data->CircleVertexPtr[s_Data->CircleVertexPtrIndex].LocalPosition = glm::vec2(s_Data->VertexPositions[i].x, s_Data->VertexPositions[i].y) * 2.0f;
			s_Data->CircleVertexPtr[s_Data->CircleVertexPtrIndex].EntityID = entityID;

			s_Data->CircleVertexPtrIndex++;
		}

		s_Data->CircleIndexCount += 6;
	}

	void BatchRenderer2D::AddLine(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::vec4& color, float thickness, int entityID)
	{
		TR_PROFILE_FUNCTION();
		if (!LineBatchHasRoom()) 
		{
			EndFrame();
			Clear();
		}

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].Position = s_Data->LineVertexPositions[i];
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].PositionA = startPoint;
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].PositionB = endPoint;
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].Color = color;
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].Thickness = thickness;
			s_Data->LineVertexPtr[s_Data->LineVertexPtrIndex].EntityID = entityID;
			s_Data->LineVertexPtrIndex++;
		}

		s_Data->LineIndexCount += 6;
	}

	void BatchRenderer2D::AddDebugLine(const glm::vec3& startPoint, const glm::vec3& endPoint, const glm::vec4& color)
	{
		TR_PROFILE_FUNCTION();
		/*if (!DebugLineBatchHasRoom())
		{
			EndFrame();
			Clear();
		}*/

		s_Data->DebugLineVertexPtr[s_Data->DebugLineVertexPtrIndex].Position = startPoint;
		s_Data->DebugLineVertexPtr[s_Data->DebugLineVertexPtrIndex].Color = color;
		s_Data->DebugLineVertexPtrIndex++;

		s_Data->DebugLineVertexPtr[s_Data->DebugLineVertexPtrIndex].Position = endPoint;
		s_Data->DebugLineVertexPtr[s_Data->DebugLineVertexPtrIndex].Color = color;
		s_Data->DebugLineVertexPtrIndex++;
	}

	//void BatchRenderer2D::AddLine(const glm::vec3 points[], int pointCount, const glm::vec4& color, float thickness)
	//{
	//	TR_PROFILE_FUNCTION();
	//	int timesToAdd = pointCount / 2;

	//	for (size_t i = 0; i < timesToAdd; i++) 
	//	{
	//		int ind = i + 1 * i;
	//		//AddLine(points[ind], points[ind + 1], color, thickness);
	//	}
	//}

	static glm::vec2 CalculateTextSize(float size, const std::string& text, Shared<FontAtlas> fontAtlas) 
	{
		TR_PROFILE_FUNCTION();
		glm::vec2 textSize = { 0.0f, 0.0f };
		float lineWidth = 0.0f;

		for (const char& c : text)
		{
			if (c == '\n') 
			{
				textSize.x = lineWidth;
				lineWidth = 0.0f;
				continue;
			}
			else if (c == '\t') 
			{
				continue;
			}

			GlyphData glyph = fontAtlas->GetGlyphData(c);

			lineWidth += glyph.Advance * size;
		}

		if (textSize.x < lineWidth)
			textSize.x = lineWidth;

		return textSize;
	}

	void BatchRenderer2D::AddText(glm::mat4& transform, const std::string& text, const glm::vec4& color, 
		Shared<FontAtlas> fontAtlas, float lineSpacing, float lineWidth, int entityID)
	{
		TR_PROFILE_FUNCTION();
		if (!fontAtlas || !fontAtlas->GetTexture())
			return;

		if (!TextBatchHasRoom())
		{
			// Begin New Batch
			EndFrame();
			Clear();
		}

		int texIndex = -1;

		for (size_t i = 0; i < s_Data->TextTextureIndex; i++)
		{
			if (s_Data->TextTextures[i] == fontAtlas->GetTexture())
			{
				texIndex = i;
				break;
			}
		}

		if (texIndex == -1)
		{
			texIndex = s_Data->TextTextureIndex;
			s_Data->TextTextures[s_Data->TextTextureIndex] = fontAtlas->GetTexture();
			s_Data->TextTextureIndex++;
		}
		
		glm::vec3 position, rotation, scale;
		Math::Decompose(transform, position, rotation, scale);

		glm::vec3 cursorOrigin = { 0.0f, 0.0f, 0.0f };
		glm::vec3 cursorPos = cursorOrigin;

		char previousChar = 0;
		size_t wordOffset = 0;

		for (const char& c : text)
		{
			// NOTE: this is a very very basic implementation of word wrapping; it wont be permenant
			if (c == ' ')
			{
				size_t separatorInd = text.find(' ', wordOffset);

				std::string currWord = text.substr(wordOffset, separatorInd - wordOffset);
				
				wordOffset = separatorInd;

				glm::vec2 wordSize = CalculateTextSize(scale.x, currWord, fontAtlas);

				if (cursorPos.x + wordSize.x > lineWidth)
				{
					cursorPos.x = cursorOrigin.x;
					cursorPos.y -= lineSpacing;
					continue;
				}
			}

			if (c == '\n') 
			{
				cursorPos.x = cursorOrigin.x;
				cursorPos.y -= lineSpacing;
				continue;
			}
			else if (c == '\t') 
			{
				GlyphData spaceData = fontAtlas->GetGlyphData(' ');
				cursorPos.x += spaceData.Advance * 4.0f;
				continue;
			}

			if (previousChar)
				cursorPos.x += (float)fontAtlas->GetKerning(previousChar, c);

			GlyphData glyph = fontAtlas->GetGlyphData(c);

			for (size_t i = 0; i < 4; i++)
			{
				glyph.VertexPositions[i].x += cursorPos.x;
				glyph.VertexPositions[i].y += cursorPos.y;
				s_Data->TextVertexPtr[s_Data->TextVertexPtrIndex].Position = transform * glyph.VertexPositions[i];
				s_Data->TextVertexPtr[s_Data->TextVertexPtrIndex].TextColor = color;
				s_Data->TextVertexPtr[s_Data->TextVertexPtrIndex].TextureCoordinates = glyph.UVs[i];
				s_Data->TextVertexPtr[s_Data->TextVertexPtrIndex].TextureIndex = texIndex;
				s_Data->TextVertexPtr[s_Data->TextVertexPtrIndex].EntityID = entityID;

				s_Data->TextVertexPtrIndex++;
			}

			s_Data->TextIndexCount += 6;

			cursorPos.x += glyph.Advance;
			previousChar = c;
		}
	}

	void BatchRenderer2D::ResetStats() { memset(&(s_Data->Stats), 0, sizeof(BatchRendererStats)); }
	BatchRendererStats BatchRenderer2D::GetStats() { return s_Data->Stats; }

	void BatchRenderer2D::AddDebugRect(const glm::mat4& transform, const glm::vec4& color)
	{
		TR_PROFILE_FUNCTION();
		glm::vec3 linePositions[4];

		linePositions[0] = transform * glm::vec4({ -0.5f, -0.5f, 0.0f, 1.0f });
		linePositions[1] = transform * glm::vec4({	0.5f, -0.5f, 0.0f, 1.0f });
		linePositions[2] = transform * glm::vec4({	0.5f,  0.5f, 0.0f, 1.0f });
		linePositions[3] = transform * glm::vec4({ -0.5f,  0.5f, 0.0f, 1.0f });
		
		AddDebugLine(linePositions[0], linePositions[1], color);
		AddDebugLine(linePositions[2], linePositions[1], color);
		AddDebugLine(linePositions[3], linePositions[2], color);
		AddDebugLine(linePositions[0], linePositions[3], color);
	}

	void BatchRenderer2D::EndFrame()
	{
		TR_PROFILE_FUNCTION();
		s_Data->Stats.VertexCount += s_Data->QuadVertexPtrIndex + s_Data->CircleVertexPtrIndex + 
									s_Data->LineVertexPtrIndex + s_Data->DebugLineVertexPtrIndex;

		s_Data->Stats.IndexCount +=  s_Data->QuadIndexCount + s_Data->CircleIndexCount + s_Data->LineIndexCount;

		// Submit quads
		if (s_Data->QuadIndexCount)
		{
			s_Data->QuadShader->Bind();
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexPtr, s_Data->QuadVertexPtrIndex * sizeof(QuadVertex));
			s_Data->QuadVertexArray->Bind();

			for (size_t i = 0; i < s_Data->QuadTextureIndex; i++)
				s_Data->QuadTextures[i]->Bind(i);

			RenderCommand::Draw(RenderMode::Triangles, s_Data->QuadVertexArray, s_Data->QuadIndexCount);

			s_Data->Stats.DrawCalls++;

			s_Data->QuadShader->Unbind();
		}

		// Submit circles
		if (s_Data->CircleIndexCount) 
		{
			s_Data->CircleShader->Bind();
			s_Data->CircleVertexBuffer->SetData(s_Data->CircleVertexPtr, s_Data->CircleVertexPtrIndex * sizeof(CircleVertex));
			s_Data->CircleVertexArray->Bind();

			RenderCommand::Draw(RenderMode::Triangles, s_Data->CircleVertexArray, s_Data->CircleIndexCount);

			s_Data->Stats.DrawCalls++;

			s_Data->CircleShader->Unbind();
		}

		// Submit lines
		if (s_Data->LineIndexCount) 
		{
			s_Data->LineShader->Bind();
			s_Data->LineVertexArray->Bind();
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertexPtr, s_Data->LineVertexPtrIndex * sizeof(LineVertex));

			RenderCommand::Draw(RenderMode::Triangles, s_Data->LineVertexArray, s_Data->LineIndexCount);
			
			s_Data->Stats.DrawCalls++;

			s_Data->LineShader->Unbind();
		}

		// Submit text
		if (s_Data->TextIndexCount) 
		{
			s_Data->TextShader->Bind();
			s_Data->TextVertexBuffer->SetData(s_Data->TextVertexPtr, s_Data->TextVertexPtrIndex * sizeof(TextVertex));
			s_Data->TextVertexArray->Bind();

			for (size_t i = 0; i < s_Data->TextTextureIndex; i++)
				s_Data->TextTextures[i]->Bind(i);

			RenderCommand::Draw(RenderMode::Triangles, s_Data->TextVertexArray, s_Data->TextIndexCount);

			s_Data->Stats.DrawCalls++;

			s_Data->TextShader->Unbind();
		}

		// Submit debug line
		if (s_Data->DebugLineVertexPtrIndex) 
		{
			s_Data->DebugLineShader->Bind();
			s_Data->DebugLineVertexBuffer->SetData(s_Data->DebugLineVertexPtr, s_Data->DebugLineVertexPtrIndex * sizeof(DebugLineVertex));
			s_Data->DebugLineVertexArray->Bind();

			constexpr float debugLineWidth = 2.0f;
			RenderCommand::SetLineWidth(debugLineWidth);
			RenderCommand::DrawArrays(RenderMode::Lines, s_Data->DebugLineVertexPtrIndex);

			s_Data->Stats.DrawCalls++;

			s_Data->DebugLineShader->Unbind();
		}
	}

	void BatchRenderer2D::Clear()
	{
		s_Data->QuadVertexPtrIndex = 0;
		s_Data->QuadIndexCount = 0;
		s_Data->QuadTextureIndex = 1;

		s_Data->CircleVertexPtrIndex = 0;
		s_Data->CircleIndexCount = 0;

		s_Data->LineVertexPtrIndex = 0;
		s_Data->LineIndexCount = 0;

		s_Data->TextIndexCount = 0;
		s_Data->TextVertexPtrIndex = 0;
		s_Data->TextTextureIndex = 0;

		s_Data->DebugLineVertexPtrIndex = 0;
	}

	bool BatchRenderer2D::QuadBatchHasRoom() { return !(s_Data->QuadIndexCount >= s_Data->MaxIndices) && !(s_Data->QuadTextureIndex >= s_Data->MaxTextureSlots); }
	bool BatchRenderer2D::CircleBatchHasRoom() { return !(s_Data->CircleIndexCount >= s_Data->MaxIndices); }
	bool BatchRenderer2D::LineBatchHasRoom() { return !(s_Data->LineIndexCount >= s_Data->MaxIndices); }
	bool BatchRenderer2D::TextBatchHasRoom() { return !(s_Data->TextIndexCount >= s_Data->MaxIndices) && !(s_Data->TextTextureIndex >= s_Data->MaxTextureSlots); }

}

#include "trpch.h"
#include "Scene.h"

#include "Components.h"

namespace TerranEngine 
{
	Scene::Scene()
	{
	}

	entt::entity Scene::CreateEntity()
	{
		auto entity = m_Registry.create();
		AddComponent<TransformComponent>(entity);

		return entity;
	}

	void Scene::Update()
	{
		auto view = m_Registry.view<TransformComponent>();

		int it = 0;

		for (auto e : view)
		{
			auto& transformComponent = GetComponent<TransformComponent>(e);

			TR_TRACE("Entity: {0}: Position: {1}, {2}, {3}", it, transformComponent.Position.x, transformComponent.Position.y, transformComponent.Position.z);
			it++;
		}
	}
}

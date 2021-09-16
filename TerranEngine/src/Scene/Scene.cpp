#include "trpch.h"
#include "Scene.h"

namespace TerranEngine 
{
	Scene::Scene()
	{
	}

	entt::entity Scene::CreateEntity()
	{
		return m_Registry.create();
	}

	void Scene::Update()
	{
	}
}

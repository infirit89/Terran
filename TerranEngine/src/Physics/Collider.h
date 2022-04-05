#pragma once

#include <glm/glm.hpp>

class b2BodyDef;

namespace TerranEngine 
{
	class Collider 
	{
	public:
		Collider(const glm::vec2& position);
		virtual ~Collider();

		virtual void SetPosition(const glm::vec2& position) = 0;
		
	private:
		b2BodyDef* bodyDef;
		glm::mat4 transformMatrix;
	};
}
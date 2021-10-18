#pragma once

#include "Entity.h"

namespace TerranEngine 
{
	struct RelationshipComponent
	{
		size_t Children = 0;
		Entity First;
		Entity Prev;
		Entity Next;
		Entity Parent;
	};
}
#pragma once

#include "Entity.h"

namespace TerranEngine 
{
	struct RelationshipComponent
	{
		// Parent variables
		size_t Children = 0;
		Entity FirstChild;
		Entity LastChild;

		// Child variables
		Entity Previous;
		Entity Next;
		Entity Parent;
	};
}

using System;
using System.Runtime.InteropServices;

namespace Terran
{
	public class Component
	{
		internal Component(UUID id) 
		{
			Entity = new Entity(id);
		}

		public Entity Entity { internal set; get; } = null!;
	}
}

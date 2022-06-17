using System;

namespace Terran
{
	class CollisionUtils
	{
		public static bool IsCollidingWith(Entity entity1, Entity entity2)
		{
			float playerHx1 = entity1.Transform.Position.X + entity1.Transform.Scale.X;
			float playerHx2 = entity1.Transform.Position.X - entity1.Transform.Scale.X;

			float playerHy2 = entity1.Transform.Position.Y + entity1.Transform.Scale.Y;
			float playerHy1 = entity1.Transform.Position.Y - entity1.Transform.Scale.Y;

			float ballHx1 = entity2.Transform.Position.X + entity2.Transform.Scale.X;
			float ballHx2 = entity2.Transform.Position.X - entity2.Transform.Scale.X;

			float ballHy2 = entity2.Transform.Position.Y + entity2.Transform.Scale.Y;
			float ballHy1 = entity2.Transform.Position.Y - entity2.Transform.Scale.Y;

			return (ballHx2 < playerHx1 && ballHx1 > playerHx2) && (ballHy2 > playerHy1 && ballHy1 < playerHy2);
		}
	}
}

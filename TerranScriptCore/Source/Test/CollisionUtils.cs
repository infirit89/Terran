using System;

namespace TerranScriptCore
{
    class CollisionUtils
    {
        public static bool IsCollidingWith(Entity entity1, Entity entity2)
        {
            float playerHx1 = entity1.transform.Position.X + entity1.transform.Scale.X;
            float playerHx2 = entity1.transform.Position.X - entity1.transform.Scale.X;

            float playerHy2 = entity1.transform.Position.Y + entity1.transform.Scale.Y;
            float playerHy1 = entity1.transform.Position.Y - entity1.transform.Scale.Y;

            float ballHx1 = entity2.transform.Position.X + entity2.transform.Scale.X;
            float ballHx2 = entity2.transform.Position.X - entity2.transform.Scale.X;

            float ballHy2 = entity2.transform.Position.Y + entity2.transform.Scale.Y;
            float ballHy1 = entity2.transform.Position.Y - entity2.transform.Scale.Y;

            return (ballHx2 < playerHx1 && ballHx1 > playerHx2) && (ballHy2 > playerHy1 && ballHy1 < playerHy2);
        }
    }
}

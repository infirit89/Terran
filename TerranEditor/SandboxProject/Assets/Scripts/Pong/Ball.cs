using System;
using Terran;

namespace Pong
{
    class Ball : Scriptable
    {
        private Entity m_Player1, m_Player2;

        internal Vector3 ballDirection;

        private Entity m_TopWall, m_BottomWall;

        private Random m_RNG;
        protected override void Init()
        {
            ballDirection = new Vector3(-0.1f, 0.0f, 0.0f);
            m_Player1 = Entity.FindWithName("Player 1");
            m_Player2 = Entity.FindWithName("Player 2");

            m_TopWall = Entity.FindWithName("Top Wall");
            m_BottomWall = Entity.FindWithName("Bottom Wall");

            m_RNG = new Random();
        }

        protected override void Update()
        {
            if (GameStateManager.CurrentGameState == GameState.WonScreen) return;

            if (CollisionUtils.IsCollidingWith(m_Player1, Entity) || CollisionUtils.IsCollidingWith(m_Player2, Entity))
            {
                int random = m_RNG.Next(2);
                ballDirection.Y = random % 2 == 0 ? -0.1f : 0.1f;
                ballDirection = -ballDirection;
                Entity.Transform.Position += ballDirection;
            }

            if (CollisionUtils.IsCollidingWith(m_TopWall, Entity) || CollisionUtils.IsCollidingWith(m_BottomWall, Entity))
                ballDirection = new Vector3(ballDirection.X, -ballDirection.Y, ballDirection.Z);

            Entity.Transform.Position += ballDirection;
        }
    }
}

using Terran;
using System;

namespace Pong 
{
    class Player : Scriptable
    {
        public string UpKey = "w", DownKey = "s";
        private KeyCode m_UpKey, m_DownKey;
        private Entity TopWall, BottomWall;

        protected override void Init()
        {
            m_UpKey = (KeyCode)Enum.Parse(typeof(KeyCode), UpKey, true);
            m_DownKey = (KeyCode)Enum.Parse(typeof(KeyCode), DownKey, true);

            TopWall = Entity.FindWithName("Top Wall");
            BottomWall = Entity.FindWithName("Bottom Wall");
        }

        protected override void Update(float deltaTime)
        {
            if (GameStateManager.CurrentGameState == GameState.WonScreen) return;
            
            if (CollisionUtils.IsCollidingWith(BottomWall, Entity))
            {
                float newYPos = BottomWall.Transform.Position.Y + (BottomWall.Transform.Scale.Y * 0.5f) + 
                                (Entity.Transform.Scale.Y * 0.5f) + 0.01f;
                Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
            }
            else if (CollisionUtils.IsCollidingWith(TopWall, Entity))
            {
                float newYPos = TopWall.Transform.Position.Y - (TopWall.Transform.Scale.Y * 0.5f) - 
                                (Entity.Transform.Scale.Y * 0.5f) - 0.01f;
                Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
            }

            if (Input.IsKeyDown(m_UpKey))
                Entity.Transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
            else if (Input.IsKeyDown(m_DownKey))
                Entity.Transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
        }
    }
}
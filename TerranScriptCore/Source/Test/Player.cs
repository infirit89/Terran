using System;

namespace TerranScriptCore
{
    class Player1 : Scriptable
    {
        private Entity m_TopWall, m_BottomWall;

        void Init() 
        {
            m_TopWall = Entity.FindWithName("Top Wall");
            m_BottomWall = Entity.FindWithName("Bottom Wall");
        }

        void Update() 
        {
            if (CollisionUtils.IsCollidingWith(m_BottomWall, entity))
            {
                float newYPos = m_BottomWall.transform.Position.Y + m_BottomWall.transform.Scale.Y + entity.transform.Scale.Y + 0.01f;
                entity.transform.Position = new Vector3(entity.transform.Position.X, newYPos, 0.0f);
            }
            else if (CollisionUtils.IsCollidingWith(m_TopWall, entity)) 
            {
                float newYPos = m_TopWall.transform.Position.Y - m_TopWall.transform.Scale.Y - entity.transform.Scale.Y - 0.01f;
                entity.transform.Position = new Vector3(entity.transform.Position.X, newYPos, 0.0f);
            }

            if (Input.KeyPressed(KeyCode.W))
                entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
            else if (Input.KeyPressed(KeyCode.S))
                entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
        }
    }

    class Player2 : Scriptable 
    {
        private Entity m_TopWall, m_BottomWall;

        void Init() 
        {
            m_TopWall = Entity.FindWithName("Top Wall");
            m_BottomWall = Entity.FindWithName("Bottom Wall");
        }

        void Update() 
        {
            if (CollisionUtils.IsCollidingWith(m_BottomWall, entity))
            {
                float newYPos = m_BottomWall.transform.Position.Y + m_BottomWall.transform.Scale.Y + entity.transform.Scale.Y + 0.01f;
                entity.transform.Position = new Vector3(entity.transform.Position.X, newYPos, 0.0f);
            }
            else if (CollisionUtils.IsCollidingWith(m_TopWall, entity))
            {
                float newYPos = m_TopWall.transform.Position.Y - m_TopWall.transform.Scale.Y - entity.transform.Scale.Y - 0.01f;
                entity.transform.Position = new Vector3(entity.transform.Position.X, newYPos, 0.0f);
            }

            if (Input.KeyPressed(KeyCode.Up))
                entity.transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
            else if (Input.KeyPressed(KeyCode.Down))
                entity.transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
        }
    }
}

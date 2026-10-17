using System;

namespace Terran
{
#if false
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
			if (CollisionUtils.IsCollidingWith(m_BottomWall, Entity))
			{
				float newYPos = m_BottomWall.Transform.Position.Y + m_BottomWall.Transform.Scale.Y + Entity.Transform.Scale.Y + 0.01f;
				Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
			}
			else if (CollisionUtils.IsCollidingWith(m_TopWall, Entity)) 
			{
				float newYPos = m_TopWall.Transform.Position.Y - m_TopWall.Transform.Scale.Y - Entity.Transform.Scale.Y - 0.01f;
				Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
			}

			if (Input.IsKeyPressed(KeyCode.W))
				Entity.Transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
			else if (Input.IsKeyPressed(KeyCode.S))
				Entity.Transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
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
			if (CollisionUtils.IsCollidingWith(m_BottomWall, Entity))
			{
				float newYPos = m_BottomWall.Transform.Position.Y + m_BottomWall.Transform.Scale.Y + Entity.Transform.Scale.Y + 0.01f;
				Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
			}
			else if (CollisionUtils.IsCollidingWith(m_TopWall, Entity))
			{
				float newYPos = m_TopWall.Transform.Position.Y - m_TopWall.Transform.Scale.Y - Entity.Transform.Scale.Y - 0.01f;
				Entity.Transform.Position = new Vector3(Entity.Transform.Position.X, newYPos, 0.0f);
			}

			if (Input.IsKeyPressed(KeyCode.Up))
				Entity.Transform.Position += new Vector3(0.0f, 0.1f, 0.0f);
			else if (Input.IsKeyPressed(KeyCode.Down))
				Entity.Transform.Position -= new Vector3(0.0f, 0.1f, 0.0f);
		}
	}
#endif
}

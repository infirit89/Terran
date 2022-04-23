using System;

namespace TerranScriptCore
{
	class Ball : Scriptable
	{
		private Entity m_Player1, m_Player2;

		internal Vector3 ballDirection;

		private Entity m_TopWall, m_BottomWall;

		private Random m_RNG;
		void Init() 
		{
			ballDirection = new Vector3(-0.1f, 0.0f, 0.0f);
			m_Player1 = Entity.FindWithName("Player 1");
			m_Player2 = Entity.FindWithName("Player 2");

			m_TopWall = Entity.FindWithName("Top Wall");
			m_BottomWall = Entity.FindWithName("Bottom Wall");

			m_RNG = new Random();
		}

		void Update() 
		{
			if (CollisionUtils.IsCollidingWith(m_Player1, entity) || CollisionUtils.IsCollidingWith(m_Player2, entity))
			{
				int random = m_RNG.Next(2);
				ballDirection.Y = random % 2 == 0 ? -0.1f : 0.1f;
				ballDirection = -ballDirection;
				entity.transform.Position += ballDirection;
			}

			if(CollisionUtils.IsCollidingWith(m_TopWall, entity) || CollisionUtils.IsCollidingWith(m_BottomWall, entity))
				ballDirection = new Vector3(ballDirection.X, -ballDirection.Y, ballDirection.Z);

			entity.transform.Position += ballDirection;
		}
	}
}

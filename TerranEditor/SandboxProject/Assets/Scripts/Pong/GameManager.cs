using Terran;

namespace Pong
{
    class GameManager : Scriptable
    {
        private Entity m_LeftWall, m_RightWall;

        private Entity m_Ball;

        public Vector3 ballCenter = new Vector3(0.0f, 0.0f, 0.0f);

        private int m_Player1Score = 0;
        private int m_Player2Score = 0;
        private Ball m_BallScript;

        protected override void Init()
        {
            m_Ball = Entity.FindWithName("Ball");

            m_LeftWall = Entity.FindWithName("Left Wall");
            m_RightWall = Entity.FindWithName("Right Wall");

            m_Ball.Transform.Position = ballCenter;

            m_BallScript = m_Ball.GetComponent<Ball>();
        }

        protected override void Update()
        {
            if (CollisionUtils.IsCollidingWith(m_RightWall, m_Ball) || CollisionUtils.IsCollidingWith(m_LeftWall, m_Ball))
            {
                if (m_Ball.Transform.Position.X < 0.0f)
                    m_Player2Score++;
                else
                    m_Player1Score++;

                m_Ball.Transform.Position = ballCenter;
                //m_BallScript.ballDirection = new Vector3(-0.1f, 0.0f, 0.0f);
                Log.Trace("Player 1 Score: {0} | Player 2 Score: {1}", m_Player1Score, m_Player2Score);
            }
        }
    }

}

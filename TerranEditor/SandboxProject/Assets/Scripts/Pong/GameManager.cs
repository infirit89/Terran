using System;
using Terran;

namespace Pong
{
    enum GameState 
    {
        Game = 0,
        WonScreen
    }

    static class GameStateManager 
    {
        public static GameState CurrentGameState = GameState.Game;
    }

    class GameManager : Scriptable
    {
        private Entity m_LeftWall, m_RightWall;

        private Entity m_Ball;
//        private TextRenderer m_Score1Renderer, m_Score2Renderer;
        //private TextRenderer m_GameWonTextRenderer;

        public Vector3 ballCenter = new Vector3(0.0f, 0.0f, 0.0f);
        public int maxPoints = 5;

        private int m_Player1Score = 0;
        private int m_Player2Score = 0;
        private Ball m_BallScript;
        private Random m_Rng;

        protected override void Init()
        {
            m_Ball = Entity.FindWithName("Ball");

            m_LeftWall = Entity.FindWithName("Left Wall");
            m_RightWall = Entity.FindWithName("Right Wall");

            m_Ball.Transform.Position = ballCenter;

            m_BallScript = m_Ball.GetComponent<Ball>();
            m_Rng = new Random();

            //m_Score1Renderer = Entity.FindWithName("Score 1").GetComponent<TextRenderer>();
            //m_Score2Renderer = Entity.FindWithName("Score 2").GetComponent<TextRenderer>();

            //m_Score1Renderer.Text = "0";
            //m_Score2Renderer.Text = "0";

            //m_GameWonTextRenderer = Entity.FindWithName("Game Won").GetComponent<TextRenderer>();
            //m_GameWonTextRenderer.Text = "";
            //GameStateManager.CurrentGameState = GameState.Game;
        }

        protected override void Update()
        {

            //if (GameStateManager.CurrentGameState == GameState.WonScreen)
            //{
            //    //m_Ball.Transform.Position = ballCenter;
            //    //m_BallScript.ballDirection = new Vector3(0.0f, 0.0f, 0.0f);

            //    if(Input.IsKeyPressed(KeyCode.R))
            //        Reset();
            //}

            if (CollisionUtils.IsCollidingWith(m_RightWall, m_Ball) || CollisionUtils.IsCollidingWith(m_LeftWall, m_Ball))
            {
                if (m_Ball.Transform.Position.X < 0.0f)
                {
                    m_Player2Score++;
                    //m_Score2Renderer.Text = m_Player2Score.ToString();
                    //if (m_Player2Score >= maxPoints) 
                    //{
                    //    GameStateManager.CurrentGameState = GameState.WonScreen;
                    //    m_GameWonTextRenderer.Text = $"Player 2 won! Points: {m_Player2Score}\n\tPress R to restart";
                    //}
                }
                else
                {
                    m_Player1Score++;
                    //m_Score1Renderer.Text = m_Player1Score.ToString();

                    //if(m_Player1Score >= maxPoints)
                    //{
                    //    GameStateManager.CurrentGameState = GameState.WonScreen;
                    //    m_GameWonTextRenderer.Text = $"Player 1 won! Points: {m_Player1Score}\n\tPress R to restart";
                    //}
                }

                m_Ball.Transform.Position = ballCenter;
                m_BallScript.ballDirection = new Vector3(m_Rng.Next(0, 2) == 0 ? -0.1f : 0.1f, 0.0f, 0.0f);
                Log.Trace("Player 1 Score: {0} | Player 2 Score: {1}", m_Player1Score, m_Player2Score);
            }
        }

        private void Reset() 
        {
            //GameStateManager.CurrentGameState = GameState.Game;
            //m_GameWonTextRenderer.Text = "";
            //m_Player1Score = 0;
            //m_Player2Score = 0;

            //m_Score1Renderer.Text = "0";
            //m_Score2Renderer.Text = "0";

            //m_BallScript.ballDirection = new Vector3(m_Rng.Next(0, 2) == 0 ? -0.1f : 0.1f, 0.0f, 0.0f);
        }
    }

}

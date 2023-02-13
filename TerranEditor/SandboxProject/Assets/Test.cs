using System;
using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        public int A = 1;
		public float B = 2.0f;
		public bool C = true;
		private TextRenderer m_TextRenderer;
        protected override void Init()
        {
            m_TextRenderer = Entity.GetComponent<TextRenderer>();
        }

        protected override void Update(float deltaTime)
        {
            if (Input.IsKeyPressed(KeyCode.A))
                m_TextRenderer.Text = "Todd Howard";
        }
    }
}

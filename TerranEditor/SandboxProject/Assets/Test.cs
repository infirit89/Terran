using System;
using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        private TextRenderer m_TextRenderer;
        protected override void Init()
        {
            m_TextRenderer = Entity.GetComponent<TextRenderer>();
        }

        protected override void Update()
        {
            if (Input.IsKeyPressed(KeyCode.A))
                m_TextRenderer.Text = "Todd Howard";
        }
    }
}

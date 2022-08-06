using System;
using Terran;

namespace SandboxProject 
{
    public enum TestEnum 
    {
        None = 0,
        Test1,
        Test2
    }

    public class Sandbox : Scriptable 
    {
        //public TestEnum Test = TestEnum.Test1;

        protected override void Init()
        {
            Log.Trace("brtuh");
            //Log.Trace("{0}", string.Format(" ", m_Test.GetArr));
        }

        protected override void Update()
        {
            if(Input.IsKeyDown(KeyCode.A))
                Log.Trace("key down");

            if (Input.IsKeyPressed(KeyCode.S))
                Log.Trace("key pressed");

            if (Input.IsKeyReleased(KeyCode.D))
                Log.Trace("key released");

            if (Input.IsMouseButtonDown(MouseButton.Button1))
                Log.Trace("button down");

            if (Input.IsMouseButtonPressed(MouseButton.Button2))
                Log.Trace("button pressed");

            if (Input.IsMouseButtonReleased(MouseButton.Button3))
                Log.Trace("button released");
        }
    }

    public class TestSandbox : Scriptable 
    {
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update()
        {
            base.Update();
        }
    }
}
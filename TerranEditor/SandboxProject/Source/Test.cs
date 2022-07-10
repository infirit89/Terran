using System;
using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        public int[] Test;

        protected override void Init()
        {
            for (int i = 0; i < Test.Length; i++)
                Log.Trace(Test[i]);
            //Log.Trace("{0}", string.Format(" ", m_Test.GetArr));
        }

        protected override void Update()
        {
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
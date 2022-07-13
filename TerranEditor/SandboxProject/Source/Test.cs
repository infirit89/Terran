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
        public TestEnum Test = TestEnum.Test1;

        protected override void Init()
        {
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
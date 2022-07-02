using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        public Entity TestEntity;
        protected override void Init()
        {
            Log.Trace(TestEntity.Name);
            
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
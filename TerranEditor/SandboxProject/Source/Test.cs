using Terran;

namespace SandboxProject 
{
    public class Sandbox : Scriptable 
    {
        public int Test = 0;
        public int Bruh = 109;
        public int Bruh2 = 109;
        public int Bruh3 = 109;
        public int Bruh4 = 109;
        public int Bruh5 = 109;
        protected override void Init()
        {
            Log.Trace(Test);
            Log.Trace(entity.HasComponent<SpriteRenderer>());
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
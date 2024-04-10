using Terran;

namespace ScriptAssembly
{
    internal class Test2 : Scriptable
    {
        public int Test = 10;

        protected override void Init()
        {
            base.Init();
            Log.Trace("chep");
        }

        protected override void Update(float deltaTime)
        {
            base.Update(deltaTime);
            Log.Trace("chep");
        }
    }
}

using Terran;

namespace ScriptAssembly
{
    class Test2 : Scriptable
    {
        public float test = 2.05f;
        protected override void Init()
        {
        }

        protected override void Update(float deltaTime)
        {
            Log.Trace("cum");
            Log.Trace(test);
        }
    }
}

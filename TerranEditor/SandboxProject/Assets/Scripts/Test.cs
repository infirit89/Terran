using Terran;

namespace ScriptAssembly
{
    internal class Test : Scriptable
    {
        public Entity Tomaaaaa;

        protected override void Init()
        {
            Test2 test2 = Tomaaaaa.GetComponent<Test2>();
            Log.Trace(test2.Test);
        }
    }
}

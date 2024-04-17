using Terran;
using TerranScriptCore.Source.Graphics;

namespace ScriptAssembly
{
    internal class Test : Scriptable
    {
        public Entity Tomaaaaa;

        [field: SerializeField]
        public int TestI { get; set; }

        protected override void Init()
        {
        }

        protected override void Update(float deltaTime)
        {
            Log.Warn(Window.Width);
            Log.Warn(Window.Height);
        }
    }
}

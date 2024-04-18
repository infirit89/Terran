using Terran;

namespace ScriptAssembly
{
    internal class Test : Scriptable
    {
        public Entity Tomaaaaa;

        [field: SerializeField]
        public int TestI { get; set; }

        protected override void Init()
        {
            //Entity mainCamera = Scene.MainCamera;
            //Log.Trace(mainCamera.Name);
        }

        protected override void Update(float deltaTime)
        {
        }
    }
#nullable disable
}

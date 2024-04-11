using Terran;

namespace ScriptAssembly.Assets.Scripts
{
    internal class Test5 : Scriptable
    {
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            base.Update(deltaTime);
            if (Input.IsMouseButtonDown(MouseButton.LeftButton))
            {

            }
        }
    }
}

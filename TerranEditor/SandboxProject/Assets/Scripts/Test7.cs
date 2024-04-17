using Terran;
using TerranScriptCore.Source.Graphics;

namespace ScriptAssembly
{
    internal class Test7 : Scriptable
    {
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            var input = Input.GetMousePosition() + new Vector2(Window.Width / 2, Window.Height / 2);

            Entity.Transform.Position = new Vector3(input.X, input.Y, 1);

            if (input.X % 2 == 0)
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Cyan;
            }
            else
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Blue;
            }

            if (input.Y % 2 == 0)
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Red;
            }
            else
            {
                Entity.GetComponent<SpriteRenderer>().Color = Color.Magenta;
            }
        }
    }
}

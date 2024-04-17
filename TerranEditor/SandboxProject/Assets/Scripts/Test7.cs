using System;
using Terran;
using TerranScriptCore.Source.Graphics;

namespace ScriptAssembly
{
    internal class Test7 : Scriptable
    {
        public float MouseSensitivity = 0.01f;
        protected override void Init()
        {
            base.Init();
        }

        protected override void Update(float deltaTime)
        {
            var input = Input.GetMousePosition() + new Vector2(Window.Width / 2, Window.Height / 2) * MouseSensitivity;
            Log.Trace(input.X);
            input.X = Math.Clamp(input.X, -Window.Width / 2, Window.Width / 2);
            Log.Trace(input.X);
            input.Y = Math.Clamp(input.Y, -Window.Height / 2, Window.Height / 2);

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
